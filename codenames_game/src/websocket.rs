use axum::{
    extract::{
        ws::{Message, WebSocket, WebSocketUpgrade},
        State,
    },
    response::Response,
};
use futures_util::{SinkExt, StreamExt};
use serde::{Deserialize, Serialize};
use std::sync::Arc;
use tracing::{debug, error, info, warn};
use uuid::Uuid;

use crate::{
    game::{GamePhase, GameRoom, GuessResult, Player, Role, Team, TurnPhase},
    llm::LlmClient,
    transcript::append_to_transcript,
    AppState,
};

/// Messages sent from client to server
#[derive(Debug, Clone, Deserialize)]
#[serde(tag = "type", rename_all = "snake_case")]
pub enum ClientMessage {
    /// Join a game room
    JoinRoom {
        room_id: Uuid,
        player_name: String,
    },
    /// Leave the current room
    LeaveRoom,
    /// Set player's team
    SetTeam {
        team: Team,
    },
    /// Set player's role
    SetRole {
        role: Role,
    },
    /// Start the game (host only)
    StartGame,
    /// Give a clue (spymaster only)
    GiveClue {
        word: String,
        number: u8,
    },
    /// Make a guess (operative only)
    MakeGuess {
        position: usize,
    },
    /// Pass turn (operative only)
    PassTurn,
    /// Request AI to take action
    RequestAiAction {
        player_id: Uuid,
    },
    /// Chat message
    Chat {
        message: String,
    },
    /// Request current game state
    RequestState,
    /// Reset the game (host only)
    ResetGame,
}

/// Messages sent from server to client
#[derive(Debug, Clone, Serialize)]
#[serde(tag = "type", rename_all = "snake_case")]
pub enum ServerMessage {
    /// Connection established
    Connected {
        player_id: Uuid,
    },
    /// Room joined successfully
    RoomJoined {
        room_id: Uuid,
        player_id: Uuid,
    },
    /// Full game state update
    GameState {
        room: RoomState,
    },
    /// Player joined the room
    PlayerJoined {
        player: PlayerInfo,
    },
    /// Player left the room
    PlayerLeft {
        player_id: Uuid,
    },
    /// Player updated their team/role
    PlayerUpdated {
        player: PlayerInfo,
    },
    /// Game started
    GameStarted,
    /// Clue given
    ClueGiven {
        team: Team,
        word: String,
        number: u8,
    },
    /// Guess made
    GuessMade {
        team: Team,
        position: usize,
        result: GuessResultInfo,
    },
    /// Turn passed
    TurnPassed {
        team: Team,
    },
    /// Game ended
    GameEnded {
        winner: Team,
    },
    /// Chat message
    ChatMessage {
        player_name: String,
        message: String,
        timestamp: String,
    },
    /// Error occurred
    Error {
        message: String,
    },
    /// AI is thinking
    AiThinking {
        player_id: Uuid,
        action: String,
    },
    /// Game reset
    GameReset,
}

#[derive(Debug, Clone, Serialize)]
pub struct RoomState {
    pub room_id: Uuid,
    pub game_name: String,
    pub players: Vec<PlayerInfo>,
    pub host_id: Uuid,
    pub game: GameStateInfo,
}

#[derive(Debug, Clone, Serialize)]
pub struct PlayerInfo {
    pub id: Uuid,
    pub name: String,
    pub team: Option<Team>,
    pub role: Option<Role>,
    pub is_ai: bool,
}

#[derive(Debug, Clone, Serialize)]
pub struct GameStateInfo {
    pub phase: GamePhase,
    pub current_team: Team,
    pub turn_phase: TurnPhase,
    pub cards: Vec<CardInfo>,
    pub current_clue: Option<ClueInfo>,
    pub clue_history: Vec<ClueInfo>,
    pub red_remaining: u8,
    pub blue_remaining: u8,
    pub starting_team: Team,
    pub winner: Option<Team>,
}

#[derive(Debug, Clone, Serialize)]
pub struct CardInfo {
    pub word: String,
    pub position: usize,
    pub revealed: bool,
    pub color: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
pub struct ClueInfo {
    pub word: String,
    pub number: u8,
    pub team: Team,
    pub guesses_made: u8,
}

#[derive(Debug, Clone, Serialize)]
pub struct GuessResultInfo {
    pub word: String,
    pub color: String,
    pub correct: bool,
    pub game_over: bool,
    pub winner: Option<Team>,
}

/// Connection state for a WebSocket client
#[derive(Debug)]
struct ConnectionState {
    player_id: Uuid,
    room_id: Option<Uuid>,
}

pub async fn ws_handler(
    ws: WebSocketUpgrade,
    State(state): State<Arc<AppState>>,
) -> Response {
    ws.on_upgrade(|socket| handle_socket(socket, state))
}

async fn handle_socket(socket: WebSocket, state: Arc<AppState>) {
    let (mut sender, mut receiver) = socket.split();

    let player_id = Uuid::new_v4();
    let mut conn_state = ConnectionState {
        player_id,
        room_id: None,
    };

    // Send connected message
    let connected_msg = ServerMessage::Connected { player_id };
    if let Err(e) = sender.send(Message::Text(serde_json::to_string(&connected_msg).unwrap().into())).await {
        error!("Failed to send connected message: {}", e);
        return;
    }

    info!("New WebSocket connection: {}", player_id);

    // Subscribe to broadcast channel
    let mut broadcast_rx = state.broadcast_tx.subscribe();

    loop {
        tokio::select! {
            // Handle incoming messages from client
            msg = receiver.next() => {
                match msg {
                    Some(Ok(Message::Text(text))) => {
                        debug!("Received message from {}: {}", player_id, text);

                        match serde_json::from_str::<ClientMessage>(&text) {
                            Ok(client_msg) => {
                                let response = handle_client_message(
                                    &state,
                                    &mut conn_state,
                                    client_msg,
                                ).await;

                                // Send response(s) to client
                                for msg in response {
                                    let json = serde_json::to_string(&msg).unwrap();
                                    if let Err(e) = sender.send(Message::Text(json.into())).await {
                                        error!("Failed to send message: {}", e);
                                        break;
                                    }
                                }
                            }
                            Err(e) => {
                                warn!("Invalid message format: {}", e);
                                let error_msg = ServerMessage::Error {
                                    message: format!("Invalid message format: {}", e),
                                };
                                let _ = sender.send(Message::Text(serde_json::to_string(&error_msg).unwrap().into())).await;
                            }
                        }
                    }
                    Some(Ok(Message::Close(_))) => {
                        info!("Client {} disconnected", player_id);
                        break;
                    }
                    Some(Err(e)) => {
                        error!("WebSocket error for {}: {}", player_id, e);
                        break;
                    }
                    None => {
                        info!("Client {} stream ended", player_id);
                        break;
                    }
                    _ => {}
                }
            }

            // Handle broadcast messages
            broadcast = broadcast_rx.recv() => {
                match broadcast {
                    Ok((target_room, msg)) => {
                        // Only send if player is in the target room
                        if conn_state.room_id == Some(target_room) {
                            let json = serde_json::to_string(&msg).unwrap();
                            if let Err(e) = sender.send(Message::Text(json.into())).await {
                                error!("Failed to send broadcast: {}", e);
                                break;
                            }
                        }
                    }
                    Err(e) => {
                        debug!("Broadcast recv error: {}", e);
                    }
                }
            }
        }
    }

    // Clean up on disconnect
    if let Some(room_id) = conn_state.room_id {
        let mut rooms = state.rooms.lock().await;
        if let Some(room) = rooms.get_mut(&room_id) {
            let player_name = room.get_player(player_id)
                .map(|p| p.name.clone())
                .unwrap_or_else(|| "Unknown".to_string());

            room.remove_player(player_id);

            // Broadcast player left
            let msg = ServerMessage::PlayerLeft { player_id };
            let _ = state.broadcast_tx.send((room_id, msg));

            // Log to transcript
            let _ = append_to_transcript(room_id, format!("**{}** left the room.", player_name)).await;

            // Remove empty rooms
            if room.players.is_empty() {
                rooms.remove(&room_id);
                info!("Removed empty room {}", room_id);
            }
        }
    }
}

async fn handle_client_message(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
    msg: ClientMessage,
) -> Vec<ServerMessage> {
    match msg {
        ClientMessage::JoinRoom { room_id, player_name } => {
            handle_join_room(state, conn_state, room_id, player_name).await
        }
        ClientMessage::LeaveRoom => {
            handle_leave_room(state, conn_state).await
        }
        ClientMessage::SetTeam { team } => {
            handle_set_team(state, conn_state, team).await
        }
        ClientMessage::SetRole { role } => {
            handle_set_role(state, conn_state, role).await
        }
        ClientMessage::StartGame => {
            handle_start_game(state, conn_state).await
        }
        ClientMessage::GiveClue { word, number } => {
            handle_give_clue(state, conn_state, word, number).await
        }
        ClientMessage::MakeGuess { position } => {
            handle_make_guess(state, conn_state, position).await
        }
        ClientMessage::PassTurn => {
            handle_pass_turn(state, conn_state).await
        }
        ClientMessage::RequestAiAction { player_id } => {
            handle_ai_action(state, conn_state, player_id).await
        }
        ClientMessage::Chat { message } => {
            handle_chat(state, conn_state, message).await
        }
        ClientMessage::RequestState => {
            handle_request_state(state, conn_state).await
        }
        ClientMessage::ResetGame => {
            handle_reset_game(state, conn_state).await
        }
    }
}

async fn handle_join_room(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
    room_id: Uuid,
    player_name: String,
) -> Vec<ServerMessage> {
    let mut rooms = state.rooms.lock().await;

    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => {
            return vec![ServerMessage::Error {
                message: "Room not found".to_string(),
            }];
        }
    };

    let player = Player::new_human(player_name.clone());
    let player_id = player.id;

    // Update connection state's player_id to match the new player
    conn_state.player_id = player_id;

    if let Err(e) = room.add_player(player.clone()) {
        return vec![ServerMessage::Error {
            message: e.to_string(),
        }];
    }

    conn_state.room_id = Some(room_id);

    // Get room state for the joining player
    let room_state = build_room_state(room, conn_state.player_id);

    // Broadcast player joined to others
    let player_info = PlayerInfo {
        id: player_id,
        name: player_name.clone(),
        team: None,
        role: None,
        is_ai: false,
    };

    let _ = state.broadcast_tx.send((room_id, ServerMessage::PlayerJoined {
        player: player_info,
    }));

    // Log to transcript
    let _ = append_to_transcript(room_id, format!("**{}** joined the room.", player_name)).await;

    vec![
        ServerMessage::RoomJoined { room_id, player_id },
        ServerMessage::GameState { room: room_state },
    ]
}

async fn handle_leave_room(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
) -> Vec<ServerMessage> {
    if let Some(room_id) = conn_state.room_id.take() {
        let mut rooms = state.rooms.lock().await;
        if let Some(room) = rooms.get_mut(&room_id) {
            room.remove_player(conn_state.player_id);

            let _ = state.broadcast_tx.send((room_id, ServerMessage::PlayerLeft {
                player_id: conn_state.player_id,
            }));
        }
    }

    vec![]
}

async fn handle_set_team(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
    team: Team,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let mut rooms = state.rooms.lock().await;
    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    if let Some(player) = room.get_player_mut(conn_state.player_id) {
        player.team = Some(team);

        let player_info = PlayerInfo {
            id: player.id,
            name: player.name.clone(),
            team: player.team,
            role: player.role,
            is_ai: player.is_ai,
        };

        let _ = state.broadcast_tx.send((room_id, ServerMessage::PlayerUpdated {
            player: player_info.clone(),
        }));

        // Log to transcript
        let _ = append_to_transcript(room_id, format!("**{}** joined the **{:?}** team.", player.name, team)).await;

        vec![]
    } else {
        vec![ServerMessage::Error {
            message: "Player not found".to_string(),
        }]
    }
}

async fn handle_set_role(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
    role: Role,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let mut rooms = state.rooms.lock().await;
    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    // First, check if player exists and get their team
    let player_info_opt = room.get_player(conn_state.player_id).map(|p| (p.id, p.team));

    let (player_id, player_team) = match player_info_opt {
        Some(info) => info,
        None => return vec![ServerMessage::Error {
            message: "Player not found".to_string(),
        }],
    };

    // Check if trying to be spymaster when one already exists for that team
    if role == Role::Spymaster {
        if let Some(team) = player_team {
            let existing_spymaster = room.players.values().any(|p| {
                p.id != player_id && p.team == Some(team) && p.role == Some(Role::Spymaster)
            });
            if existing_spymaster {
                return vec![ServerMessage::Error {
                    message: "Team already has a spymaster".to_string(),
                }];
            }
        }
    }

    // Now we can safely get mutable access
    if let Some(player) = room.get_player_mut(conn_state.player_id) {
        player.role = Some(role);

        let player_info = PlayerInfo {
            id: player.id,
            name: player.name.clone(),
            team: player.team,
            role: player.role,
            is_ai: player.is_ai,
        };

        let _ = state.broadcast_tx.send((room_id, ServerMessage::PlayerUpdated {
            player: player_info,
        }));

        // Log to transcript
        let _ = append_to_transcript(room_id, format!("**{}** became the **{:?}**.", player.name, role)).await;

        vec![]
    } else {
        vec![ServerMessage::Error {
            message: "Player not found".to_string(),
        }]
    }
}

async fn handle_start_game(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let mut rooms = state.rooms.lock().await;
    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    // Check if player is host
    if room.host_id != conn_state.player_id {
        return vec![ServerMessage::Error {
            message: "Only the host can start the game".to_string(),
        }];
    }

    // Check if game can start
    if !room.can_start() {
        return vec![ServerMessage::Error {
            message: "Each team needs at least one player and a spymaster".to_string(),
        }];
    }

    if let Err(e) = room.game.start_game() {
        return vec![ServerMessage::Error {
            message: e.to_string(),
        }];
    }

    // Broadcast game started
    let _ = state.broadcast_tx.send((room_id, ServerMessage::GameStarted));

    // Log to transcript
    let _ = append_to_transcript(room_id, "Game started.".to_string()).await;

    // Send updated game state to all
    let room_state = build_room_state(room, conn_state.player_id);
    let _ = state.broadcast_tx.send((room_id, ServerMessage::GameState { room: room_state }));

    vec![]
}

async fn handle_give_clue(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
    word: String,
    number: u8,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let mut rooms = state.rooms.lock().await;
    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    // Verify player is spymaster of current team
    let (player_name, player_role, player_team) = match room.get_player(conn_state.player_id) {
        Some(p) => (p.name.clone(), p.role, p.team),
        None => return vec![ServerMessage::Error {
            message: "Player not found".to_string(),
        }],
    };

    if player_role != Some(Role::Spymaster) {
        return vec![ServerMessage::Error {
            message: "Only spymasters can give clues".to_string(),
        }];
    }

    if player_team != Some(room.game.current_team) {
        return vec![ServerMessage::Error {
            message: "It's not your team's turn".to_string(),
        }];
    }

    let team = room.game.current_team;

    if let Err(e) = room.game.give_clue(team, word.clone(), number) {
        return vec![ServerMessage::Error {
            message: e.to_string(),
        }];
    }

    // Broadcast clue given
    let _ = state.broadcast_tx.send((room_id, ServerMessage::ClueGiven {
        team,
        word: word.clone(),
        number,
    }));

    // Log to transcript
    let log = format!("**{} ({:?} Spymaster)** gives clue: `{} {}`", player_name, team, word, number);
    let _ = append_to_transcript(room_id, log).await;

    vec![]
}

async fn handle_make_guess(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
    position: usize,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let mut rooms = state.rooms.lock().await;
    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    // Verify player is operative of current team
    let (player_name, player_role, player_team) = match room.get_player(conn_state.player_id) {
        Some(p) => (p.name.clone(), p.role, p.team),
        None => return vec![ServerMessage::Error {
            message: "Player not found".to_string(),
        }],
    };

    if player_role != Some(Role::Operative) {
        return vec![ServerMessage::Error {
            message: "Only operatives can make guesses".to_string(),
        }];
    }

    if player_team != Some(room.game.current_team) {
        return vec![ServerMessage::Error {
            message: "It's not your team's turn".to_string(),
        }];
    }

    let team = room.game.current_team;

    let result = match room.game.make_guess(team, position) {
        Ok(result) => result,
        Err(e) => return vec![ServerMessage::Error {
            message: e.to_string(),
        }],
    };

    let card = &room.game.cards[position];
    let result_info = match &result {
        GuessResult::Correct { color, word } => GuessResultInfo {
            word: word.clone(),
            color: format!("{:?}", color),
            correct: true,
            game_over: false,
            winner: None,
        },
        GuessResult::WrongTeam { color, word } => GuessResultInfo {
            word: word.clone(),
            color: format!("{:?}", color),
            correct: false,
            game_over: false,
            winner: None,
        },
        GuessResult::Neutral { word } => GuessResultInfo {
            word: word.clone(),
            color: "Neutral".to_string(),
            correct: false,
            game_over: false,
            winner: None,
        },
        GuessResult::Assassin { winner } => GuessResultInfo {
            word: card.word.clone(),
            color: "Assassin".to_string(),
            correct: false,
            game_over: true,
            winner: Some(*winner),
        },
        GuessResult::LastCard { winner, word } => GuessResultInfo {
            word: word.clone(),
            color: format!("{:?}", team),
            correct: true,
            game_over: true,
            winner: Some(*winner),
        },
    };

    // Broadcast guess made
    let _ = state.broadcast_tx.send((room_id, ServerMessage::GuessMade {
        team,
        position,
        result: result_info.clone(),
    }));

    // Log to transcript
    let result_str = if result_info.correct {
        "Correct".to_string()
    } else {
        format!("Wrong - {}", result_info.color)
    };
    let log = format!("**{} ({:?} Operative)** guesses: `{}` (Result: {})", player_name, team, result_info.word, result_str);
    let _ = append_to_transcript(room_id, log).await;

    // If game ended, broadcast game ended
    if room.game.phase == GamePhase::Finished {
        if let Some(winner) = room.game.winner {
            let _ = state.broadcast_tx.send((room_id, ServerMessage::GameEnded { winner }));
            let _ = append_to_transcript(room_id, format!("**Game Over! Winner: {:?}**", winner)).await;
        }
    }

    vec![]
}

async fn handle_pass_turn(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let mut rooms = state.rooms.lock().await;
    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    let (player_name, player_team) = match room.get_player(conn_state.player_id) {
        Some(p) => (p.name.clone(), p.team),
        None => return vec![ServerMessage::Error {
            message: "Player not found".to_string(),
        }],
    };

    if player_team != Some(room.game.current_team) {
        return vec![ServerMessage::Error {
            message: "It's not your team's turn".to_string(),
        }];
    }

    let team = room.game.current_team;

    if let Err(e) = room.game.pass_turn(team) {
        return vec![ServerMessage::Error {
            message: e.to_string(),
        }];
    }

    // Broadcast turn passed
    let _ = state.broadcast_tx.send((room_id, ServerMessage::TurnPassed { team }));

    // Log to transcript
    let _ = append_to_transcript(room_id, format!("**{} ({:?})** ended their turn.", player_name, team)).await;

    vec![]
}

async fn handle_ai_action(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
    ai_player_id: Uuid,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    // Get room data and AI player info
    let (ai_config, team, role, player_name, game_view, current_clue) = {
        let rooms = state.rooms.lock().await;
        let room = match rooms.get(&room_id) {
            Some(room) => room,
            None => return vec![ServerMessage::Error {
                message: "Room not found".to_string(),
            }],
        };

        let player = match room.get_player(ai_player_id) {
            Some(p) => p,
            None => return vec![ServerMessage::Error {
                message: "AI player not found".to_string(),
            }],
        };

        if !player.is_ai {
            return vec![ServerMessage::Error {
                message: "Player is not an AI".to_string(),
            }];
        }

        let ai_config = match &player.ai_config {
            Some(config) => config.clone(),
            None => return vec![ServerMessage::Error {
                message: "AI player has no configuration".to_string(),
            }],
        };

        let team = match player.team {
            Some(t) => t,
            None => return vec![ServerMessage::Error {
                message: "AI player has no team".to_string(),
            }],
        };

        let role = match player.role {
            Some(r) => r,
            None => return vec![ServerMessage::Error {
                message: "AI player has no role".to_string(),
            }],
        };

        let player_name = player.name.clone();

        // Check if it's this AI's turn
        if team != room.game.current_team {
            return vec![ServerMessage::Error {
                message: "It's not the AI's team's turn".to_string(),
            }];
        }

        let is_spymaster = role == Role::Spymaster;
        let game_view = room.game.get_view(is_spymaster);
        let current_clue = room.game.current_clue.clone();

        (ai_config, team, role, player_name, game_view, current_clue)
    };

    // Get API key
    let api_key = ai_config.api_key
        .or_else(|| state.default_api_key.clone())
        .unwrap_or_default();

    if api_key.is_empty() {
        return vec![ServerMessage::Error {
            message: "No API key configured for AI player".to_string(),
        }];
    }

    // Broadcast that AI is thinking
    let action = match role {
        Role::Spymaster => "generating clue",
        Role::Operative => "making guess",
    };
    let _ = state.broadcast_tx.send((room_id, ServerMessage::AiThinking {
        player_id: ai_player_id,
        action: action.to_string(),
    }));

    // Perform AI action
    let llm_client = LlmClient::new();

    match role {
        Role::Spymaster => {
            match llm_client.generate_clue(&api_key, &ai_config.model, &game_view, team).await {
                Ok((word, number, debug_info)) => {
                    // Log AI thought process
                    let thought_log = format!(
                        "### AI Thinking ({} - {} Spymaster - {})\n**Prompt:**\n{}\n\n**Response:**\n{}",
                        player_name,
                        match team { Team::Red => "Red", Team::Blue => "Blue" },
                        ai_config.model,
                        debug_info.prompt,
                        debug_info.response
                    );
                    let _ = append_to_transcript(room_id, thought_log).await;

                    // Give the clue
                    let mut rooms = state.rooms.lock().await;
                    if let Some(room) = rooms.get_mut(&room_id) {
                        if let Err(e) = room.game.give_clue(team, word.clone(), number) {
                            return vec![ServerMessage::Error {
                                message: e.to_string(),
                            }];
                        }

                        let _ = state.broadcast_tx.send((room_id, ServerMessage::ClueGiven {
                            team,
                            word: word.clone(),
                            number,
                        }));

                        let _ = append_to_transcript(room_id, format!("**{} ({:?} Spymaster AI)** gives clue: `{} {}`", player_name, team, word, number)).await;
                    }
                }
                Err(e) => {
                    return vec![ServerMessage::Error {
                        message: format!("AI failed to generate clue: {}", e),
                    }];
                }
            }
        }
        Role::Operative => {
            let clue = match current_clue {
                Some(c) => c,
                None => return vec![ServerMessage::Error {
                    message: "No clue has been given".to_string(),
                }],
            };

            match llm_client.make_guess(&api_key, &ai_config.model, &game_view, team, &clue).await {
                Ok((position, debug_info)) => {
                    // Log AI thought process
                    let thought_log = format!(
                        "### AI Thinking ({} - {} Operative - {})\n**Prompt:**\n{}\n\n**Response:**\n{}",
                        player_name,
                        match team { Team::Red => "Red", Team::Blue => "Blue" },
                        ai_config.model,
                        debug_info.prompt,
                        debug_info.response
                    );
                    let _ = append_to_transcript(room_id, thought_log).await;

                    // Make the guess
                    let mut rooms = state.rooms.lock().await;
                    if let Some(room) = rooms.get_mut(&room_id) {
                        let result = match room.game.make_guess(team, position) {
                            Ok(r) => r,
                            Err(e) => {
                                return vec![ServerMessage::Error {
                                    message: e.to_string(),
                                }];
                            }
                        };

                        let card = &room.game.cards[position];
                        let result_info = match &result {
                            GuessResult::Correct { color, word } => GuessResultInfo {
                                word: word.clone(),
                                color: format!("{:?}", color),
                                correct: true,
                                game_over: false,
                                winner: None,
                            },
                            GuessResult::WrongTeam { color, word } => GuessResultInfo {
                                word: word.clone(),
                                color: format!("{:?}", color),
                                correct: false,
                                game_over: false,
                                winner: None,
                            },
                            GuessResult::Neutral { word } => GuessResultInfo {
                                word: word.clone(),
                                color: "Neutral".to_string(),
                                correct: false,
                                game_over: false,
                                winner: None,
                            },
                            GuessResult::Assassin { winner } => GuessResultInfo {
                                word: card.word.clone(),
                                color: "Assassin".to_string(),
                                correct: false,
                                game_over: true,
                                winner: Some(*winner),
                            },
                            GuessResult::LastCard { winner, word } => GuessResultInfo {
                                word: word.clone(),
                                color: format!("{:?}", team),
                                correct: true,
                                game_over: true,
                                winner: Some(*winner),
                            },
                        };

                        let _ = state.broadcast_tx.send((room_id, ServerMessage::GuessMade {
                            team,
                            position,
                            result: result_info.clone(),
                        }));

                        // Log to transcript
                        let result_str = if result_info.correct {
                            "Correct".to_string()
                        } else {
                            format!("Wrong - {}", result_info.color)
                        };
                        let log = format!("**{} ({:?} Operative AI)** guesses: `{}` (Result: {})", player_name, team, result_info.word, result_str);
                        let _ = append_to_transcript(room_id, log).await;

                        if room.game.phase == GamePhase::Finished {
                            if let Some(winner) = room.game.winner {
                                let _ = state.broadcast_tx.send((room_id, ServerMessage::GameEnded { winner }));
                                let _ = append_to_transcript(room_id, format!("**Game Over! Winner: {:?}**", winner)).await;
                            }
                        }
                    }
                }
                Err(e) => {
                    return vec![ServerMessage::Error {
                        message: format!("AI failed to make guess: {}", e),
                    }];
                }
            }
        }
    }

    vec![]
}

async fn handle_chat(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
    message: String,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let rooms = state.rooms.lock().await;
    let room = match rooms.get(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    let player_name = room.get_player(conn_state.player_id)
        .map(|p| p.name.clone())
        .unwrap_or_else(|| "Unknown".to_string());

    let timestamp = chrono::Utc::now().format("%H:%M:%S").to_string();

    let _ = state.broadcast_tx.send((room_id, ServerMessage::ChatMessage {
        player_name: player_name.clone(),
        message: message.clone(),
        timestamp,
    }));

    // Log to transcript
    let _ = append_to_transcript(room_id, format!("**{}:** {}", player_name, message)).await;

    vec![]
}

async fn handle_request_state(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let rooms = state.rooms.lock().await;
    let room = match rooms.get(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    let room_state = build_room_state(room, conn_state.player_id);

    vec![ServerMessage::GameState { room: room_state }]
}

async fn handle_reset_game(
    state: &Arc<AppState>,
    conn_state: &mut ConnectionState,
) -> Vec<ServerMessage> {
    let room_id = match conn_state.room_id {
        Some(id) => id,
        None => return vec![ServerMessage::Error {
            message: "Not in a room".to_string(),
        }],
    };

    let mut rooms = state.rooms.lock().await;
    let room = match rooms.get_mut(&room_id) {
        Some(room) => room,
        None => return vec![ServerMessage::Error {
            message: "Room not found".to_string(),
        }],
    };

    // Check if player is host
    if room.host_id != conn_state.player_id {
        return vec![ServerMessage::Error {
            message: "Only the host can reset the game".to_string(),
        }];
    }

    // Create new game state
    room.game = crate::game::GameState::new(room.game.name.clone());

    // Reset player roles
    for player in room.players.values_mut() {
        player.role = None;
    }

    // Broadcast reset
    let _ = state.broadcast_tx.send((room_id, ServerMessage::GameReset));

    let room_state = build_room_state(room, conn_state.player_id);
    let _ = state.broadcast_tx.send((room_id, ServerMessage::GameState { room: room_state }));

    vec![]
}

fn build_room_state(room: &GameRoom, viewer_id: Uuid) -> RoomState {
    let viewer = room.get_player(viewer_id);
    let is_spymaster = viewer
        .map(|p| p.role == Some(Role::Spymaster))
        .unwrap_or(false);

    let game_view = room.game.get_view(is_spymaster);

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
            red_remaining: game_view.red_remaining,
            blue_remaining: game_view.blue_remaining,
            starting_team: game_view.starting_team,
            winner: game_view.winner,
        },
    }
}
