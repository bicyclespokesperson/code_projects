use crate::token;

pub struct Scanner {
    source: String,
    tokens: Vec<token::Token>,
}

impl Scanner {
    pub fn new(source: String) -> Scanner {
        Scanner {
            source,
            tokens: vec![],
        }
    }
}
