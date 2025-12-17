# Codenames Game

A multiplayer Codenames game written in Rust that supports both human players and AI agents (LLMs).

## Features

- **Multiplayer Support**: Up to 10 players per room
- **Human & AI Players**: Mix human players with LLM-powered AI agents
- **Multiple LLM Providers**: Use any model via OpenRouter (GPT-4, Claude, Gemini, Llama, Mistral, etc.)
- **Real-time Updates**: WebSocket-based communication for instant game state sync
- **Beautiful UI**: Modern, responsive browser interface
- **Full Game Rules**: Complete Codenames implementation with spymasters and operatives

## Quick Start

### Prerequisites

- Rust 1.70+ (install from [rustup.rs](https://rustup.rs))
- (Optional) OpenRouter API key for AI players

### Running the Game

```bash
cd codenames_game

# Build and run
cargo run --release

# Or with an API key for AI players
OPENROUTER_API_KEY=sk-or-... cargo run --release
```

The server starts at `http://localhost:3000`

### Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `PORT` | Server port | 3000 |
| `OPENROUTER_API_KEY` | Default API key for AI players | None |

You can also set the API key in the browser UI under "AI Configuration".

## How to Play

### Creating a Game

1. Open `http://localhost:3000` in your browser
2. Enter a room name and your player name
3. Click "Create Room"

### Joining a Game

1. Open the game URL
2. Enter your name
3. Click on an available room in the list

### Setting Up Teams

1. Click "Join Red" or "Join Blue" to pick your team
2. Choose your role: **Spymaster** or **Operative**
3. Each team needs at least one spymaster
4. Add AI players if desired (requires API key)

### Game Rules

**Spymaster:**
- You can see all card colors
- Give one-word clues with a number indicating how many cards relate to it
- Your clue cannot be a word on the board

**Operative:**
- Guess cards based on your spymaster's clue
- You can guess up to (clue number + 1) cards
- Click "End Turn" to pass to the other team

**Card Colors:**
- 🔴 Red: Red team's words
- 🔵 Blue: Blue team's words
- 🟤 Neutral: Safe but ends your turn
- ⚫ Assassin: Instant loss!

**Winning:**
- Find all your team's words first, OR
- The opposing team hits the assassin

## AI Players

The game supports AI-powered players through [OpenRouter](https://openrouter.ai).

### Supported Models

- OpenAI: GPT-4o, GPT-4o Mini, GPT-4 Turbo
- Anthropic: Claude 3.5 Sonnet, Claude 3 Opus, Claude 3 Haiku
- Google: Gemini Pro 1.5, Gemini Flash 1.5
- Meta: Llama 3.1 70B, Llama 3.1 8B
- Mistral: Mistral Large, Mistral Small

### Adding AI Players

1. Get an API key from [OpenRouter](https://openrouter.ai)
2. Enter the key in "AI Configuration" or set `OPENROUTER_API_KEY`
3. In the lobby, use the "Add AI Player" section
4. Select team, role, and model
5. Click "Add AI"

When it's an AI player's turn, click "Trigger AI Action" to have them play.

## Architecture

```
codenames_game/
├── src/
│   ├── main.rs        # Server entry point
│   ├── game.rs        # Core game logic and state
│   ├── llm.rs         # OpenRouter LLM integration
│   ├── websocket.rs   # WebSocket handlers
│   └── api.rs         # HTTP API endpoints
├── static/
│   ├── index.html     # Main UI
│   ├── styles.css     # Styling
│   └── app.js         # Client-side logic
└── Cargo.toml
```

## API Reference

### HTTP Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/health` | Health check |
| GET | `/api/rooms` | List all rooms |
| POST | `/api/rooms` | Create a new room |
| GET | `/api/rooms/:id` | Get room details |
| POST | `/api/rooms/:id/ai-player` | Add AI player |
| DELETE | `/api/rooms/:id/players/:pid` | Remove player |
| GET | `/api/models` | List available AI models |

### WebSocket Messages

Connect to `/ws` for real-time game updates. See `src/websocket.rs` for message types.

## Development

```bash
# Run with hot reload (requires cargo-watch)
cargo watch -x run

# Run tests
cargo test

# Build for production
cargo build --release
```

## License

MIT
