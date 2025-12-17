use anyhow::{anyhow, Result};
use serde::{Deserialize, Serialize};
use tracing::{debug, error, info};

use crate::game::{CardColor, Clue, GameView, Team};

/// OpenRouter API client for LLM interactions
#[derive(Debug, Clone)]
pub struct LlmClient {
    client: reqwest::Client,
    base_url: String,
}

#[derive(Debug, Deserialize)]
struct OpenRouterModelsResponse {
    data: Vec<OpenRouterModel>,
}

#[derive(Debug, Deserialize)]
struct OpenRouterModel {
    id: String,
    name: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LlmDebugInfo {
    pub prompt: String,
    pub response: String,
}

impl LlmClient {
    pub fn new() -> Self {
        Self {
            client: reqwest::Client::new(),
            base_url: "https://openrouter.ai/api/v1".to_string(),
        }
    }

    /// Fetch available models from OpenRouter
    pub async fn fetch_models(&self) -> Result<Vec<ModelInfo>> {
        let response = self.client
            .get(format!("{}/models", self.base_url))
            .send()
            .await?;

        if !response.status().is_success() {
            let status = response.status();
            let body = response.text().await.unwrap_or_default();
            error!("OpenRouter API error fetching models: {} - {}", status, body);
            return Err(anyhow!("API request failed: {} - {}", status, body));
        }

        let response: OpenRouterModelsResponse = response.json().await?;

        let mut models: Vec<ModelInfo> = response.data
            .into_iter()
            .map(|m| {
                let provider = m.id.split('/').next().unwrap_or("Unknown").to_string();
                // Capitalize first letter of provider
                let provider = provider.chars()
                    .next()
                    .map(|c| c.to_uppercase().to_string() + &provider[1..])
                    .unwrap_or(provider);

                ModelInfo {
                    id: m.id,
                    name: m.name,
                    provider,
                }
            })
            .collect();

        // Sort models by provider, then by name
        models.sort_by(|a, b| {
            match a.provider.cmp(&b.provider) {
                std::cmp::Ordering::Equal => a.name.cmp(&b.name),
                other => other,
            }
        });

        Ok(models)
    }

    /// Generate a clue as a spymaster
    pub async fn generate_clue(
        &self,
        api_key: &str,
        model: &str,
        game_view: &GameView,
        team: Team,
    ) -> Result<(String, u8, LlmDebugInfo)> {
        let prompt = build_spymaster_prompt(game_view, team);

        info!("Generating clue for {:?} team using model {}", team, model);
        debug!("Spymaster prompt: {}", prompt);

        let response = self.chat_completion(api_key, model, &prompt).await?;

        info!("LLM response: {}", response);

        let debug_info = LlmDebugInfo {
            prompt,
            response: response.clone(),
        };

        // Parse the response to extract clue word and number
        let (word, number) = parse_clue_response(&response)?;
        Ok((word, number, debug_info))
    }

    /// Make a guess as an operative
    pub async fn make_guess(
        &self,
        api_key: &str,
        model: &str,
        game_view: &GameView,
        team: Team,
        clue: &Clue,
    ) -> Result<(usize, LlmDebugInfo)> {
        let prompt = build_operative_prompt(game_view, team, clue);

        info!("Generating guess for {:?} team using model {}", team, model);
        debug!("Operative prompt: {}", prompt);

        let response = self.chat_completion(api_key, model, &prompt).await?;

        info!("LLM response: {}", response);

        let debug_info = LlmDebugInfo {
            prompt,
            response: response.clone(),
        };

        // Parse the response to extract the chosen word position
        let position = parse_guess_response(&response, game_view)?;
        Ok((position, debug_info))
    }

    async fn chat_completion(&self, api_key: &str, model: &str, prompt: &str) -> Result<String> {
        let request = OpenRouterRequest {
            model: model.to_string(),
            messages: vec![
                Message {
                    role: "system".to_string(),
                    content: "You are playing Codenames, a word association game. Follow the instructions precisely and respond in the exact format requested.".to_string(),
                },
                Message {
                    role: "user".to_string(),
                    content: prompt.to_string(),
                },
            ],
            max_tokens: Some(256),
            temperature: Some(0.7),
        };

        let response = self.client
            .post(format!("{}/chat/completions", self.base_url))
            .header("Authorization", format!("Bearer {}", api_key))
            .header("Content-Type", "application/json")
            .header("HTTP-Referer", "https://codenames-game.local")
            .header("X-Title", "Codenames Game")
            .json(&request)
            .send()
            .await?;

        if !response.status().is_success() {
            let status = response.status();
            let body = response.text().await.unwrap_or_default();
            error!("OpenRouter API error: {} - {}", status, body);
            return Err(anyhow!("API request failed: {} - {}", status, body));
        }

        let response: OpenRouterResponse = response.json().await?;

        response.choices
            .first()
            .map(|c| c.message.content.clone())
            .ok_or_else(|| anyhow!("No response from LLM"))
    }
}

impl Default for LlmClient {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug, Serialize)]
struct OpenRouterRequest {
    model: String,
    messages: Vec<Message>,
    #[serde(skip_serializing_if = "Option::is_none")]
    max_tokens: Option<u32>,
    #[serde(skip_serializing_if = "Option::is_none")]
    temperature: Option<f32>,
}

#[derive(Debug, Serialize, Deserialize)]
struct Message {
    role: String,
    content: String,
}

#[derive(Debug, Deserialize)]
struct OpenRouterResponse {
    choices: Vec<Choice>,
}

#[derive(Debug, Deserialize)]
struct Choice {
    message: Message,
}

fn build_spymaster_prompt(game_view: &GameView, team: Team) -> String {
    let team_color = match team {
        Team::Red => "RED",
        Team::Blue => "BLUE",
    };
    let opponent_color = match team {
        Team::Red => "BLUE",
        Team::Blue => "RED",
    };

    let mut team_words = Vec::new();
    let mut opponent_words = Vec::new();
    let mut neutral_words = Vec::new();
    let mut assassin_word = String::new();
    let mut revealed_words = Vec::new();

    for card in &game_view.cards {
        if card.revealed {
            revealed_words.push(card.word.clone());
            continue;
        }

        if let Some(color) = card.color {
            match (color, team) {
                (CardColor::Red, Team::Red) | (CardColor::Blue, Team::Blue) => {
                    team_words.push(card.word.clone());
                }
                (CardColor::Red, Team::Blue) | (CardColor::Blue, Team::Red) => {
                    opponent_words.push(card.word.clone());
                }
                (CardColor::Neutral, _) => {
                    neutral_words.push(card.word.clone());
                }
                (CardColor::Assassin, _) => {
                    assassin_word = card.word.clone();
                }
            }
        }
    }

    let clue_history = if game_view.clue_history.is_empty() {
        "No clues given yet.".to_string()
    } else {
        game_view.clue_history
            .iter()
            .map(|c| format!("{:?}: {} {}", c.team, c.word, c.number))
            .collect::<Vec<_>>()
            .join("\n")
    };

    format!(r#"You are the {team_color} SPYMASTER in Codenames.

YOUR TEAM'S WORDS (you want your operatives to guess these):
{team_words}

OPPONENT'S WORDS ({opponent_color} - avoid these):
{opponent_words}

NEUTRAL WORDS (not ideal, but not terrible):
{neutral_words}

ASSASSIN (NEVER hint at this - instant loss):
{assassin}

Already revealed words (can't be guessed):
{revealed}

Previous clues in this game:
{history}

RULES:
- Give a ONE-WORD clue and a NUMBER indicating how many words relate to it
- The clue must be a single English word
- The clue CANNOT be any word on the board
- The clue CANNOT be a word that sounds like or rhymes with a word on the board
- Try to connect multiple team words while avoiding opponent/assassin words

Respond with ONLY the clue in this exact format:
CLUE: [word] [number]

Example: CLUE: OCEAN 3"#,
        team_color = team_color,
        team_words = team_words.join(", "),
        opponent_words = opponent_words.join(", "),
        neutral_words = neutral_words.join(", "),
        assassin = assassin_word,
        revealed = if revealed_words.is_empty() { "None".to_string() } else { revealed_words.join(", ") },
        history = clue_history,
    )
}

fn build_operative_prompt(game_view: &GameView, team: Team, clue: &Clue) -> String {
    let team_color = match team {
        Team::Red => "RED",
        Team::Blue => "BLUE",
    };

    let available_words: Vec<String> = game_view.cards
        .iter()
        .filter(|c| !c.revealed)
        .map(|c| format!("{}. {}", c.position, c.word))
        .collect();

    let revealed_info: Vec<String> = game_view.cards
        .iter()
        .filter(|c| c.revealed)
        .map(|c| {
            let color = c.color.map(|col| format!("{:?}", col)).unwrap_or_default();
            format!("{} ({})", c.word, color)
        })
        .collect();

    let clue_history = game_view.clue_history
        .iter()
        .map(|c| format!("{:?}: {} {}", c.team, c.word, c.number))
        .collect::<Vec<_>>()
        .join("\n");

    format!(r#"You are a {team_color} OPERATIVE in Codenames.

The spymaster gave the clue: "{clue_word}" {clue_number}

This means there are approximately {clue_number} words on the board related to "{clue_word}".

AVAILABLE WORDS TO GUESS (position. word):
{words}

ALREADY REVEALED (for context):
{revealed}

CLUE HISTORY:
{history}

YOUR TASK:
Choose ONE word from the available words that best matches the clue "{clue_word}".
Think about which word has the strongest connection to the clue.

Respond with ONLY the position number of your guess:
GUESS: [number]

Example: GUESS: 5"#,
        team_color = team_color,
        clue_word = clue.word,
        clue_number = clue.number,
        words = available_words.join("\n"),
        revealed = if revealed_info.is_empty() { "None".to_string() } else { revealed_info.join(", ") },
        history = if clue_history.is_empty() { "None".to_string() } else { clue_history },
    )
}

fn parse_clue_response(response: &str) -> Result<(String, u8)> {
    // Try to find "CLUE: word number" pattern
    let response = response.trim();

    // Look for the CLUE: prefix
    let clue_part = if let Some(idx) = response.to_uppercase().find("CLUE:") {
        &response[idx + 5..]
    } else {
        response
    };

    let parts: Vec<&str> = clue_part.trim().split_whitespace().collect();

    if parts.len() >= 2 {
        let word = parts[0].to_uppercase().trim_matches(|c: char| !c.is_alphanumeric()).to_string();
        let number: u8 = parts[1].trim_matches(|c: char| !c.is_numeric())
            .parse()
            .unwrap_or(1);

        if word.is_empty() {
            return Err(anyhow!("Could not parse clue word from response"));
        }

        return Ok((word, number.min(9)));
    }

    // Fallback: try to extract any word and number
    let word: String = response
        .chars()
        .filter(|c| c.is_alphabetic() || *c == ' ')
        .collect::<String>()
        .split_whitespace()
        .next()
        .unwrap_or("HINT")
        .to_uppercase();

    let number: u8 = response
        .chars()
        .filter(|c| c.is_numeric())
        .collect::<String>()
        .parse()
        .unwrap_or(1)
        .min(9);

    Ok((word, number))
}

fn parse_guess_response(response: &str, game_view: &GameView) -> Result<usize> {
    let response = response.trim();

    // Try to find "GUESS: number" pattern
    let guess_part = if let Some(idx) = response.to_uppercase().find("GUESS:") {
        &response[idx + 6..]
    } else {
        response
    };

    // Try to parse a number
    let number: Option<usize> = guess_part
        .trim()
        .split_whitespace()
        .next()
        .and_then(|s| s.trim_matches(|c: char| !c.is_numeric()).parse().ok());

    if let Some(pos) = number {
        if pos < game_view.cards.len() && !game_view.cards[pos].revealed {
            return Ok(pos);
        }
    }

    // Try to find a word match
    let response_upper = response.to_uppercase();
    for card in &game_view.cards {
        if !card.revealed && response_upper.contains(&card.word.to_uppercase()) {
            return Ok(card.position);
        }
    }

    Err(anyhow!("Could not parse guess from response: {}", response))
}

/// Available models through OpenRouter
pub const AVAILABLE_MODELS: &[(&str, &str, &str)] = &[
    ("google/gemini-3-pro", "Gemini 3 Pro", "Google"),
    ("google/gemini-flash-1.5", "Gemini Flash 1.5", "Google"),
    ("anthropic/claude-4-opus", "Claude 4 Opus", "Anthropic"),
    ("anthropic/claude-4-sonnet", "Claude 4 Sonnet", "Anthropic"),
    ("openai/gpt-5", "GPT-5", "OpenAI"),
    ("openai/gpt-4o", "GPT-4o", "OpenAI"),
    ("openai/o1-preview", "OpenAI o1", "OpenAI"),
    ("meta-llama/llama-3.1-70b-instruct", "Llama 3.1 70B", "Meta"),
    ("mistralai/mistral-large", "Mistral Large", "Mistral"),
];

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ModelInfo {
    pub id: String,
    pub name: String,
    pub provider: String,
}

pub fn get_available_models() -> Vec<ModelInfo> {
    AVAILABLE_MODELS
        .iter()
        .map(|(id, name, provider)| ModelInfo {
            id: id.to_string(),
            name: name.to_string(),
            provider: provider.to_string(),
        })
        .collect()
}
