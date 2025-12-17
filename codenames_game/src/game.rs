use rand::seq::SliceRandom;
use rand::Rng;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;

/// Standard Codenames word list
pub const WORD_LIST: &[&str] = &[
    "AFRICA", "AGENT", "AIR", "ALIEN", "ALPS", "AMAZON", "AMBULANCE", "AMERICA",
    "ANGEL", "ANTARCTICA", "APPLE", "ARM", "ATLANTIS", "AUSTRALIA", "AZTEC",
    "BACK", "BALL", "BAND", "BANK", "BAR", "BARK", "BAT", "BATTERY", "BEACH",
    "BEAR", "BEAT", "BED", "BEIJING", "BELL", "BELT", "BERLIN", "BERMUDA",
    "BERRY", "BILL", "BLOCK", "BOARD", "BOLT", "BOMB", "BOND", "BOOM", "BOOT",
    "BOTTLE", "BOW", "BOX", "BRIDGE", "BRUSH", "BUCK", "BUFFALO", "BUG",
    "BUGLE", "BUTTON", "CALF", "CANADA", "CAP", "CAPITAL", "CAR", "CARD",
    "CARROT", "CASINO", "CAST", "CAT", "CELL", "CENTAUR", "CENTER", "CHAIR",
    "CHANGE", "CHARGE", "CHECK", "CHEST", "CHICK", "CHINA", "CHOCOLATE",
    "CHURCH", "CIRCLE", "CLIFF", "CLOAK", "CLUB", "CODE", "COLD", "COMIC",
    "COMPOUND", "CONCERT", "CONDUCTOR", "CONTRACT", "COOK", "COPPER", "COTTON",
    "COURT", "COVER", "CRANE", "CRASH", "CRICKET", "CROSS", "CROWN", "CYCLE",
    "CZECH", "DANCE", "DATE", "DAY", "DEATH", "DECK", "DEGREE", "DIAMOND",
    "DICE", "DINOSAUR", "DISEASE", "DOCTOR", "DOG", "DRAFT", "DRAGON", "DRESS",
    "DRILL", "DROP", "DUCK", "DWARF", "EAGLE", "EGYPT", "EMBASSY", "ENGINE",
    "ENGLAND", "EUROPE", "EYE", "FACE", "FAIR", "FALL", "FAN", "FENCE", "FIELD",
    "FIGHTER", "FIGURE", "FILE", "FILM", "FIRE", "FISH", "FLUTE", "FLY", "FOOT",
    "FORCE", "FOREST", "FORK", "FRANCE", "GAME", "GAS", "GENIUS", "GERMANY",
    "GHOST", "GIANT", "GLASS", "GLOVE", "GOLD", "GRACE", "GRASS", "GREECE",
    "GREEN", "GROUND", "HAM", "HAND", "HAWK", "HEAD", "HEART", "HELICOPTER",
    "HIDE", "HIMALAYAS", "HOLE", "HOLLYWOOD", "HONEY", "HOOD", "HOOK", "HORN",
    "HORSE", "HORSESHOE", "HOSPITAL", "HOTEL", "ICE", "ICE CREAM", "INDIA",
    "IRON", "IVORY", "JACK", "JAM", "JET", "JUPITER", "KANGAROO", "KETCHUP",
    "KEY", "KID", "KING", "KIWI", "KNIFE", "KNIGHT", "LAB", "LAP", "LASER",
    "LAWYER", "LEAD", "LEMON", "LEPRECHAUN", "LIFE", "LIGHT", "LIMOUSINE",
    "LINE", "LINK", "LION", "LITTER", "LOCH NESS", "LOCK", "LOG", "LONDON",
    "LUCK", "MAIL", "MAMMOTH", "MAPLE", "MARBLE", "MARCH", "MASS", "MATCH",
    "MERCURY", "MEXICO", "MICROSCOPE", "MILLIONAIRE", "MINE", "MINT", "MISSILE",
    "MODEL", "MOLE", "MOON", "MOSCOW", "MOUNT", "MOUSE", "MOUTH", "MUG",
    "NAIL", "NEEDLE", "NET", "NEW YORK", "NIGHT", "NINJA", "NOTE", "NOVEL",
    "NURSE", "NUT", "OCTOPUS", "OIL", "OLIVE", "OLYMPUS", "OPERA", "ORANGE",
    "ORGAN", "PALM", "PAN", "PANTS", "PAPER", "PARACHUTE", "PARK", "PART",
    "PASS", "PASTE", "PENGUIN", "PHOENIX", "PIANO", "PIE", "PILOT", "PIN",
    "PIPE", "PIRATE", "PISTOL", "PIT", "PITCH", "PLANE", "PLASTIC", "PLATE",
    "PLATYPUS", "PLAY", "PLOT", "POINT", "POISON", "POLE", "POLICE", "POOL",
    "PORT", "POST", "POUND", "PRESS", "PRINCE", "PRINCESS", "PUMPKIN", "PUPIL",
    "PYRAMID", "QUEEN", "RABBIT", "RACKET", "RAY", "REVOLUTION", "RING", "ROBIN",
    "ROBOT", "ROCK", "ROME", "ROOT", "ROSE", "ROULETTE", "ROUND", "ROW", "RULER",
    "SATELLITE", "SATURN", "SCALE", "SCHOOL", "SCIENTIST", "SCORPION", "SCREEN",
    "SCUBA DIVER", "SEAL", "SERVER", "SHADOW", "SHAKESPEARE", "SHARK", "SHED",
    "SHELL", "SHERIFF", "SHIP", "SHOE", "SHOP", "SHOT", "SHOULDER", "SILK",
    "SINK", "SKYSCRAPER", "SLIP", "SLUG", "SMUGGLER", "SNOW", "SNOWMAN", "SOCK",
    "SOLDIER", "SOUL", "SOUND", "SPACE", "SPELL", "SPIDER", "SPIKE", "SPINE",
    "SPOT", "SPRING", "SPY", "SQUARE", "STADIUM", "STAFF", "STAR", "STATE",
    "STICK", "STOCK", "STRAW", "STREAM", "STRIKE", "STRING", "SUB", "SUIT",
    "SUPERHERO", "SWING", "SWITCH", "TABLE", "TABLET", "TAG", "TAIL", "TAP",
    "TEACHER", "TELESCOPE", "TEMPLE", "THIEF", "THUMB", "TICK", "TIE", "TIGER",
    "TIME", "TOKYO", "TOOTH", "TORCH", "TOWER", "TRACK", "TRAIN", "TRIANGLE",
    "TRIP", "TRUNK", "TUBE", "TURKEY", "UNDERTAKER", "UNICORN", "VACUUM", "VAN",
    "VET", "WAKE", "WALL", "WAR", "WASHER", "WASHINGTON", "WATCH", "WATER",
    "WAVE", "WEB", "WELL", "WHALE", "WHIP", "WIND", "WITCH", "WORM", "YARD",
];

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum Team {
    Red,
    Blue,
}

impl Team {
    pub fn opposite(&self) -> Team {
        match self {
            Team::Red => Team::Blue,
            Team::Blue => Team::Red,
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum Role {
    Spymaster,
    Operative,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum CardColor {
    Red,
    Blue,
    Neutral,
    Assassin,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Card {
    pub word: String,
    pub color: CardColor,
    pub revealed: bool,
    pub position: usize,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum GamePhase {
    Lobby,
    Playing,
    Finished,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Clue {
    pub word: String,
    pub number: u8,
    pub team: Team,
    pub guesses_made: u8,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum TurnPhase {
    GivingClue,
    Guessing,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GameState {
    pub id: Uuid,
    pub name: String,
    pub cards: Vec<Card>,
    pub phase: GamePhase,
    pub current_team: Team,
    pub turn_phase: TurnPhase,
    pub current_clue: Option<Clue>,
    pub clue_history: Vec<Clue>,
    pub transcript: Vec<String>,
    pub red_remaining: u8,
    pub blue_remaining: u8,
    pub starting_team: Team,
    pub winner: Option<Team>,
    pub created_at: chrono::DateTime<chrono::Utc>,
}

impl GameState {
    pub fn new(name: String) -> Self {
        let mut rng = rand::thread_rng();

        // Randomly select starting team
        let starting_team = if rng.gen_bool(0.5) {
            Team::Red
        } else {
            Team::Blue
        };

        // Starting team gets 9 cards, other team gets 8
        let (red_count, blue_count) = match starting_team {
            Team::Red => (9u8, 8u8),
            Team::Blue => (8u8, 9u8),
        };

        // Select 25 random words
        let mut words: Vec<&str> = WORD_LIST.choose_multiple(&mut rng, 25).cloned().collect();
        words.shuffle(&mut rng);

        // Assign colors to positions
        let mut colors: Vec<CardColor> = Vec::with_capacity(25);
        colors.extend(std::iter::repeat(CardColor::Red).take(red_count as usize));
        colors.extend(std::iter::repeat(CardColor::Blue).take(blue_count as usize));
        colors.extend(std::iter::repeat(CardColor::Neutral).take(7));
        colors.push(CardColor::Assassin);
        colors.shuffle(&mut rng);

        // Create cards
        let cards: Vec<Card> = words
            .into_iter()
            .zip(colors.into_iter())
            .enumerate()
            .map(|(i, (word, color))| Card {
                word: word.to_string(),
                color,
                revealed: false,
                position: i,
            })
            .collect();

        Self {
            id: Uuid::new_v4(),
            name,
            cards,
            phase: GamePhase::Lobby,
            current_team: starting_team,
            turn_phase: TurnPhase::GivingClue,
            current_clue: None,
            clue_history: Vec::new(),
            transcript: Vec::new(),
            red_remaining: red_count,
            blue_remaining: blue_count,
            starting_team,
            winner: None,
            created_at: chrono::Utc::now(),
        }
    }

    pub fn start_game(&mut self) -> Result<(), GameError> {
        if self.phase != GamePhase::Lobby {
            return Err(GameError::InvalidPhase);
        }
        self.phase = GamePhase::Playing;
        Ok(())
    }

    pub fn give_clue(&mut self, team: Team, word: String, number: u8) -> Result<(), GameError> {
        if self.phase != GamePhase::Playing {
            return Err(GameError::InvalidPhase);
        }
        if self.current_team != team {
            return Err(GameError::NotYourTurn);
        }
        if self.turn_phase != TurnPhase::GivingClue {
            return Err(GameError::InvalidTurnPhase);
        }

        // Validate clue word
        let word_upper = word.to_uppercase();
        for card in &self.cards {
            if !card.revealed && card.word.to_uppercase() == word_upper {
                return Err(GameError::InvalidClue("Clue cannot be a word on the board".into()));
            }
        }

        let clue = Clue {
            word,
            number,
            team,
            guesses_made: 0,
        };

        self.current_clue = Some(clue.clone());
        self.clue_history.push(clue);
        self.turn_phase = TurnPhase::Guessing;

        Ok(())
    }

    pub fn make_guess(&mut self, team: Team, position: usize) -> Result<GuessResult, GameError> {
        if self.phase != GamePhase::Playing {
            return Err(GameError::InvalidPhase);
        }
        if self.current_team != team {
            return Err(GameError::NotYourTurn);
        }
        if self.turn_phase != TurnPhase::Guessing {
            return Err(GameError::InvalidTurnPhase);
        }
        if position >= self.cards.len() {
            return Err(GameError::InvalidCard);
        }

        let card = &mut self.cards[position];
        if card.revealed {
            return Err(GameError::CardAlreadyRevealed);
        }

        card.revealed = true;
        let card_color = card.color;
        let card_word = card.word.clone();

        // Update remaining counts
        match card_color {
            CardColor::Red => self.red_remaining = self.red_remaining.saturating_sub(1),
            CardColor::Blue => self.blue_remaining = self.blue_remaining.saturating_sub(1),
            _ => {}
        }

        // Update guesses made
        if let Some(ref mut clue) = self.current_clue {
            clue.guesses_made += 1;
        }

        // Check for game end conditions
        let result = match card_color {
            CardColor::Assassin => {
                self.phase = GamePhase::Finished;
                self.winner = Some(team.opposite());
                GuessResult::Assassin { winner: team.opposite() }
            }
            CardColor::Red if self.red_remaining == 0 => {
                self.phase = GamePhase::Finished;
                self.winner = Some(Team::Red);
                GuessResult::LastCard { winner: Team::Red, word: card_word }
            }
            CardColor::Blue if self.blue_remaining == 0 => {
                self.phase = GamePhase::Finished;
                self.winner = Some(Team::Blue);
                GuessResult::LastCard { winner: Team::Blue, word: card_word }
            }
            CardColor::Red if team == Team::Red => {
                GuessResult::Correct { color: CardColor::Red, word: card_word }
            }
            CardColor::Blue if team == Team::Blue => {
                GuessResult::Correct { color: CardColor::Blue, word: card_word }
            }
            CardColor::Neutral => {
                self.end_turn();
                GuessResult::Neutral { word: card_word }
            }
            _ => {
                // Wrong team's card
                self.end_turn();
                GuessResult::WrongTeam { color: card_color, word: card_word }
            }
        };

        // Check if team should continue guessing
        if matches!(result, GuessResult::Correct { .. }) {
            if let Some(ref clue) = self.current_clue {
                // Can guess clue.number + 1 times (for the bonus guess)
                if clue.guesses_made > clue.number {
                    self.end_turn();
                }
            }
        }

        Ok(result)
    }

    pub fn end_turn(&mut self) {
        self.current_team = self.current_team.opposite();
        self.turn_phase = TurnPhase::GivingClue;
        self.current_clue = None;
    }

    pub fn pass_turn(&mut self, team: Team) -> Result<(), GameError> {
        if self.phase != GamePhase::Playing {
            return Err(GameError::InvalidPhase);
        }
        if self.current_team != team {
            return Err(GameError::NotYourTurn);
        }
        if self.turn_phase != TurnPhase::Guessing {
            return Err(GameError::InvalidTurnPhase);
        }

        self.end_turn();
        Ok(())
    }

    /// Get a view of the game state for a specific role
    pub fn get_view(&self, is_spymaster: bool) -> GameView {
        let cards: Vec<CardView> = self.cards.iter().map(|card| {
            CardView {
                word: card.word.clone(),
                position: card.position,
                revealed: card.revealed,
                color: if card.revealed || is_spymaster {
                    Some(card.color)
                } else {
                    None
                },
            }
        }).collect();

        GameView {
            id: self.id,
            name: self.name.clone(),
            cards,
            phase: self.phase,
            current_team: self.current_team,
            turn_phase: self.turn_phase,
            current_clue: self.current_clue.clone(),
            clue_history: self.clue_history.clone(),
            transcript: self.transcript.clone(),
            red_remaining: self.red_remaining,
            blue_remaining: self.blue_remaining,
            starting_team: self.starting_team,
            winner: self.winner,
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CardView {
    pub word: String,
    pub position: usize,
    pub revealed: bool,
    pub color: Option<CardColor>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GameView {
    pub id: Uuid,
    pub name: String,
    pub cards: Vec<CardView>,
    pub phase: GamePhase,
    pub current_team: Team,
    pub turn_phase: TurnPhase,
    pub current_clue: Option<Clue>,
    pub clue_history: Vec<Clue>,
    pub transcript: Vec<String>,
    pub red_remaining: u8,
    pub blue_remaining: u8,
    pub starting_team: Team,
    pub winner: Option<Team>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum GuessResult {
    Correct { color: CardColor, word: String },
    WrongTeam { color: CardColor, word: String },
    Neutral { word: String },
    Assassin { winner: Team },
    LastCard { winner: Team, word: String },
}

#[derive(Debug, Clone, thiserror::Error)]
pub enum GameError {
    #[error("Invalid game phase for this action")]
    InvalidPhase,
    #[error("It's not your turn")]
    NotYourTurn,
    #[error("Invalid turn phase for this action")]
    InvalidTurnPhase,
    #[error("Invalid card position")]
    InvalidCard,
    #[error("Card already revealed")]
    CardAlreadyRevealed,
    #[error("Invalid clue: {0}")]
    InvalidClue(String),
    #[error("Player not found")]
    PlayerNotFound,
    #[error("Room not found")]
    RoomNotFound,
    #[error("Room is full")]
    RoomFull,
    #[error("Not enough players to start")]
    NotEnoughPlayers,
    #[error("Invalid role for this action")]
    InvalidRole,
    #[error("Player name '{0}' is already taken")]
    PlayerNameTaken(String),
}

impl Serialize for GameError {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        serializer.serialize_str(&self.to_string())
    }
}

/// Game room containing game state and players
#[derive(Debug, Clone)]
pub struct GameRoom {
    pub game: GameState,
    pub players: HashMap<Uuid, Player>,
    pub max_players: usize,
    pub host_id: Uuid,
}

impl GameRoom {
    pub fn new(name: String, host: Player) -> Self {
        let host_id = host.id;
        let mut players = HashMap::new();
        players.insert(host.id, host);

        Self {
            game: GameState::new(name),
            players,
            max_players: 10,
            host_id,
        }
    }

    pub fn add_player(&mut self, player: Player) -> Result<(), GameError> {
        if self.players.len() >= self.max_players {
            return Err(GameError::RoomFull);
        }
        
        // Check for duplicate names (case-insensitive)
        if self.players.values().any(|p| p.name.to_lowercase() == player.name.to_lowercase()) {
            return Err(GameError::PlayerNameTaken(player.name));
        }

        self.players.insert(player.id, player);
        Ok(())
    }

    pub fn remove_player(&mut self, player_id: Uuid) -> Option<Player> {
        self.players.remove(&player_id)
    }

    pub fn get_player(&self, player_id: Uuid) -> Option<&Player> {
        self.players.get(&player_id)
    }

    pub fn get_player_mut(&mut self, player_id: Uuid) -> Option<&mut Player> {
        self.players.get_mut(&player_id)
    }

    pub fn can_start(&self) -> bool {
        // Need at least two players on each team
        let red_count = self.players.values().filter(|p| p.team == Some(Team::Red)).count();
        let blue_count = self.players.values().filter(|p| p.team == Some(Team::Blue)).count();

        // Need at least one spymaster per team
        let red_spymaster = self.players.values().any(|p| {
            p.team == Some(Team::Red) && p.role == Some(Role::Spymaster)
        });
        let blue_spymaster = self.players.values().any(|p| {
            p.team == Some(Team::Blue) && p.role == Some(Role::Spymaster)
        });

        red_count >= 2 && blue_count >= 2 && red_spymaster && blue_spymaster
    }

    pub fn get_spymaster(&self, team: Team) -> Option<&Player> {
        self.players.values().find(|p| {
            p.team == Some(team) && p.role == Some(Role::Spymaster)
        })
    }

    pub fn get_operatives(&self, team: Team) -> Vec<&Player> {
        self.players.values()
            .filter(|p| p.team == Some(team) && p.role == Some(Role::Operative))
            .collect()
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Player {
    pub id: Uuid,
    pub name: String,
    pub team: Option<Team>,
    pub role: Option<Role>,
    pub is_ai: bool,
    pub ai_config: Option<AiConfig>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AiConfig {
    pub model: String,
    pub provider: String,
    pub api_key: Option<String>,  // If None, uses server default
}

impl Player {
    pub fn new_human(name: String) -> Self {
        Self {
            id: Uuid::new_v4(),
            name,
            team: None,
            role: None,
            is_ai: false,
            ai_config: None,
        }
    }

    pub fn new_ai(name: String, config: AiConfig) -> Self {
        Self {
            id: Uuid::new_v4(),
            name,
            team: None,
            role: None,
            is_ai: true,
            ai_config: Some(config),
        }
    }
}
