use std::path::Path;
use std::process::exit;
use std::{env, fs};

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() > 2 {
        eprintln!("Usage: {} [arg]", env::args().next().unwrap());
        exit(64);
    } else if args.len() == 2 {
        println!("Loading {}", args[0]);
        if let Err(e) = run_file(Path::new(&args[1])) {
            eprintln!("Failed to read {}: {}", args[1], e);
            exit(64);
        }
    } else {
        run_prompt();
    }
}

fn run_prompt() {}

fn run_file(path: &Path) -> std::io::Result<()> {
    let contents = fs::read_to_string(path)?;
    Ok(run(&contents))
}

fn run(source: &str) {}
