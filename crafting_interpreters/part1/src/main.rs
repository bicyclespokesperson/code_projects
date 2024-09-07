use std::io::Write;
use std::io::{self, stdout, BufRead};
use std::path::Path;
use std::process::exit;
use std::{env, fs};

fn main() {
    let mut l = Lox { had_error: false };
    l.main()
}

pub struct Lox {
    pub had_error: bool,
}

impl Lox {
    fn run(&mut self, source: &String) {
        let tokens = source.split_whitespace();

        for token in tokens {
            print!("{} ", token);
        }
    }

    fn run_prompt(&mut self) {
        let mut buf: String = String::new();
        loop {
            print!("> ");
            stdout().flush().expect("Failed to flush stdout buffer");
            match io::stdin().lock().read_line(&mut buf) {
                Err(err) => panic!("Failed to read from stdin: {err}"),
                Ok(0) => break,
                Ok(_) => self.run(&buf),
            }
            println!();
        }
    }

    fn run_file(&mut self, path: &Path) -> std::io::Result<()> {
        let contents = fs::read_to_string(path)?;
        self.run(&contents);
        Ok(())
    }

    pub fn main(&mut self) {
        let args: Vec<String> = env::args().collect();

        if args.len() > 2 {
            eprintln!("Usage: {} [arg]", args[0]);
            exit(64);
        } else if args.len() == 2 {
            println!("Loading {}", args[0]);
            if let Err(e) = self.run_file(Path::new(&args[1])) {
                eprintln!("Failed to read {}: {}", args[1], e);
                exit(64);
            }
        } else {
            self.run_prompt();
        }
    }

    fn error(&mut self, line: i32, message: &str) {
        self.report(line, "", message);
    }

    fn report(&mut self, line: i32, wheree: &str, message: &str) {
        println!("[line {}] Error{}: {}", line, wheree, message);
        self.had_error = true;
    }
}
