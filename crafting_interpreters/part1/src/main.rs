use std::io::Write;
use std::io::{self, stdout, BufRead};
use std::path::Path;
use std::process::exit;
use std::{env, fs};

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() > 2 {
        eprintln!("Usage: {} [arg]", args[0]);
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

pub struct Lox {
    had_error: bool,
}

fn run_prompt() {
    let mut buf: String = String::new();
    loop {
        print!("> ");
        stdout().flush().expect("Failed to flush stdout buffer");
        match io::stdin().lock().read_line(&mut buf) {
            Err(err) => panic!("Failed to read from stdin: {err}"),
            Ok(0) => break,
            Ok(_) => run(&buf),
        }
        println!();
    }
}

fn run_file(path: &Path) -> std::io::Result<()> {
    let contents = fs::read_to_string(path)?;
    run(&contents);
    Ok(())
}

fn run(source: &String) {
    let tokens = source.split_whitespace();

    for token in tokens {
        print!("{} ", token);
    }
}

fn error(line: i32, message: &str) {
    report(line, "", message);
}

fn report(line: i32, wheree: &str, message: &str) {
    println!("[line {}] Error{}: {}", line, wheree, message);
    // hadError = true;
}
