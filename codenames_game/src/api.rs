use axum::{
    extract::{Path, State},
    http::StatusCode,
    response::IntoResponse,
    Json,
};
use serde::{Deserialize, Serialize};
use std::sync::Arc;
use uuid::Uuid;

use crate::{
    game::{AiConfig, GameRoom, Player, Role, Team},
    llm::{get_available_models, LlmClient},
    websocket::{CardInfo, ClueInfo, GameStateInfo, PlayerInfo, RoomState, ServerMessage},
    AppState,
};

#[derive(Debug, Serialize)]
pub struct ApiResponse<T> {
    pub success: bool,
    pub data: Option<T>,
    pub error: Option<String>,
}

impl<T: Serialize> ApiResponse<T> {
    pub fn success(data: T) -> Self {
        Self {
            success: true,
            data: Some(data),
            error: None,
        }
    }

    pub fn error(message: impl Into<String>) -> ApiResponse<T> {
        ApiResponse {
            success: false,
            data: None,
            error: Some(message.into()),
        }
    }
}

#[derive(Debug, Deserialize)]
pub struct CreateRoomRequest {
    pub name: String,
    pub host_name: String,
}

#[derive(Debug, Serialize)]
pub struct CreateRoomResponse {
    pub room_id: Uuid,
    pub host_id: Uuid,
}

/// Create a new game room
pub async fn create_room(
    State(state): State<Arc<AppState>>,
    Json(req): Json<CreateRoomRequest>,
) -> impl IntoResponse {
    let host = Player::new_human(req.host_name);
    let host_id = host.id;

    let room = GameRoom::new(req.name, host);
    let room_id = room.game.id;

    let mut rooms = state.rooms.lock().await;
    rooms.insert(room_id, room);

    (
        StatusCode::OK,
        Json(ApiResponse::success(CreateRoomResponse { room_id, host_id })),
    )
}

#[derive(Debug, Serialize)]
pub struct RoomListItem {
    pub id: Uuid,
    pub name: String,
    pub player_count: usize,
    pub phase: String,
}

/// List all available rooms
pub async fn list_rooms(
    State(state): State<Arc<AppState>>,
) -> impl IntoResponse {
    let rooms = state.rooms.lock().await;

    let room_list: Vec<RoomListItem> = rooms.values().map(|room| {
        RoomListItem {
            id: room.game.id,
            name: room.game.name.clone(),
            player_count: room.players.len(),
            phase: format!("{:?}", room.game.phase),
        }
    }).collect();

    (StatusCode::OK, Json(ApiResponse::success(room_list)))
}

/// Get room details
pub async fn get_room(
    State(state): State<Arc<AppState>>,
    Path(room_id): Path<Uuid>,
) -> impl IntoResponse {
    let rooms = state.rooms.lock().await;

    match rooms.get(&room_id) {
        Some(room) => {
            let room_state = build_public_room_state(room);
            (StatusCode::OK, Json(ApiResponse::success(room_state)))
        }
        None => (
            StatusCode::NOT_FOUND,
            Json(ApiResponse::<RoomState>::error("Room not found")),
        ),
    }
}

#[derive(Debug, Deserialize)]
pub struct AddAiPlayerRequest {
    pub name: String,
    pub team: Team,
    pub role: Role,
    pub model: String,
    pub api_key: Option<String>,
}

/// Add an AI player to a room
pub async fn add_ai_player(
    State(state): State<Arc<AppState>>,
    Path(room_id): Path<Uuid>,
    Json(req): Json<AddAiPlayerRequest>,
) -> impl IntoResponse {
    let mut rooms = state.rooms.lock().await;

    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return (
            StatusCode::NOT_FOUND,
            Json(ApiResponse::<PlayerInfo>::error("Room not found")),
        ),
    };

    // Check if team already has a spymaster
    if req.role == Role::Spymaster {
        let has_spymaster = room.players.values().any(|p| {
            p.team == Some(req.team) && p.role == Some(Role::Spymaster)
        });
        if has_spymaster {
            return (
                StatusCode::BAD_REQUEST,
                Json(ApiResponse::<PlayerInfo>::error("Team already has a spymaster")),
            );
        }
    }

    let ai_config = AiConfig {
        model: req.model,
        provider: "openrouter".to_string(),
        api_key: req.api_key,
    };

    let mut player = Player::new_ai(req.name.clone(), ai_config);
    player.team = Some(req.team);
    player.role = Some(req.role);

    let player_info = PlayerInfo {
        id: player.id,
        name: player.name.clone(),
        team: player.team,
        role: player.role,
        is_ai: player.is_ai,
    };

    if let Err(e) = room.add_player(player) {
        return (
            StatusCode::BAD_REQUEST,
            Json(ApiResponse::<PlayerInfo>::error(e.to_string())),
        );
    }

    // Broadcast player joined
    let _ = state.broadcast_tx.send((room_id, ServerMessage::PlayerJoined {
        player: player_info.clone(),
    }));

    (StatusCode::OK, Json(ApiResponse::success(player_info)))
}

/// Remove a player from a room
pub async fn remove_player(
    State(state): State<Arc<AppState>>,
    Path((room_id, player_id)): Path<(Uuid, Uuid)>,
) -> impl IntoResponse {
    let mut rooms = state.rooms.lock().await;

    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return (
            StatusCode::NOT_FOUND,
            Json(ApiResponse::<()>::error("Room not found")),
        ),
    };

    if room.remove_player(player_id).is_some() {
        // Broadcast player left
        let _ = state.broadcast_tx.send((room_id, ServerMessage::PlayerLeft { player_id }));
        (StatusCode::OK, Json(ApiResponse::success(())))
    } else {
        (
            StatusCode::NOT_FOUND,
            Json(ApiResponse::<()>::error("Player not found")),
        )
    }
}

/// Get available LLM models
pub async fn get_models() -> impl IntoResponse {
    let models = match LlmClient::new().fetch_models().await {
        Ok(models) => models,
        Err(e) => {
            tracing::error!("Failed to fetch models from OpenRouter: {}", e);
            get_available_models()
        }
    };
    (StatusCode::OK, Json(ApiResponse::success(models)))
}

/// Health check endpoint
pub async fn health() -> impl IntoResponse {
    (StatusCode::OK, Json(ApiResponse::success("OK")))
}

fn build_public_room_state(room: &GameRoom) -> RoomState {
    // For public view, don't show card colors
    let game_view = room.game.get_view(false);

    let players: Vec<PlayerInfo> = room.players.values().map(|p| PlayerInfo {
        id: p.id,
        name: p.name.clone(),
        team: p.team,
        role: p.role,
        is_ai: p.is_ai,
    }).collect();

    let cards: Vec<CardInfo> = game_view.cards.iter().map(|c| CardInfo {
        word: c.word.clone(),
        position: c.position,
        revealed: c.revealed,
        color: c.color.map(|col| format!("{:?}", col)),
    }).collect();

    let current_clue = game_view.current_clue.map(|c| ClueInfo {
        word: c.word,
        number: c.number,
        team: c.team,
        guesses_made: c.guesses_made,
    });

    let clue_history: Vec<ClueInfo> = game_view.clue_history.iter().map(|c| ClueInfo {
        word: c.word.clone(),
        number: c.number,
        team: c.team,
        guesses_made: c.guesses_made,
    }).collect();

    RoomState {
        room_id: room.game.id,
        game_name: game_view.name,
        players,
        host_id: room.host_id,
        game: GameStateInfo {
            phase: game_view.phase,
            current_team: game_view.current_team,
            turn_phase: game_view.turn_phase,
            cards,
            current_clue,
            clue_history,
            transcript: game_view.transcript,
            red_remaining: game_view.red_remaining,
            blue_remaining: game_view.blue_remaining,
            starting_team: game_view.starting_team,
            winner: game_view.winner,
        },
    }
}
