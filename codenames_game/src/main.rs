mod api;
mod game;
mod llm;
mod transcript;
mod websocket;

use axum::{
    routing::{get, post, delete},
    Router,
};
use std::{collections::HashMap, net::SocketAddr, sync::Arc};
use tokio::sync::{broadcast, Mutex};
use tower_http::{
    cors::{Any, CorsLayer},
    services::ServeDir,
};
use tracing::{info, Level};
use tracing_subscriber::FmtSubscriber;
use uuid::Uuid;

use crate::game::GameRoom;
use crate::websocket::ServerMessage;

/// Application state shared across all handlers
pub struct AppState {
    /// All active game rooms
    pub rooms: Mutex<HashMap<Uuid, GameRoom>>,
    /// Broadcast channel for real-time updates
    pub broadcast_tx: broadcast::Sender<(Uuid, ServerMessage)>,
    /// Default API key for LLM calls (from environment)
    pub default_api_key: Option<String>,
}

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    // Initialize logging
    let subscriber = FmtSubscriber::builder()
        .with_max_level(Level::INFO)
        .with_target(false)
        .finish();
    tracing::subscriber::set_global_default(subscriber)?;

    // Load environment variables
    dotenvy::dotenv().ok();

    // Get default API key from environment
    let default_api_key = std::env::var("OPENROUTER_API_KEY").ok();

    // Create broadcast channel
    let (broadcast_tx, _) = broadcast::channel::<(Uuid, ServerMessage)>(1000);

    // Create shared state
    let state = Arc::new(AppState {
        rooms: Mutex::new(HashMap::new()),
        broadcast_tx,
        default_api_key,
    });

    // Build API routes
    let api_routes = Router::new()
        .route("/health", get(api::health))
        .route("/rooms", get(api::list_rooms))
        .route("/rooms", post(api::create_room))
        .route("/rooms/:room_id", get(api::get_room))
        .route("/rooms/:room_id/ai-player", post(api::add_ai_player))
        .route("/rooms/:room_id/players/:player_id", delete(api::remove_player))
        .route("/models", get(api::get_models));

    // Build main router
    let app = Router::new()
        .route("/ws", get(websocket::ws_handler))
        .nest("/api", api_routes)
        .fallback_service(ServeDir::new("static"))
        .layer(
            CorsLayer::new()
                .allow_origin(Any)
                .allow_methods(Any)
                .allow_headers(Any),
        )
        .with_state(state);

    // Start server
    let port: u16 = std::env::var("PORT")
        .ok()
        .and_then(|p| p.parse().ok())
        .unwrap_or(3000);

    let addr = SocketAddr::from(([0, 0, 0, 0], port));
    info!("🎮 Codenames server starting on http://localhost:{}", port);
    info!("📂 Serving static files from ./static");
    info!("🔌 WebSocket endpoint: ws://localhost:{}/ws", port);

    let listener = tokio::net::TcpListener::bind(addr).await?;
    axum::serve(listener, app).await?;

    Ok(())
}
