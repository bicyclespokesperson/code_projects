// This file is the crate root

// Allow unused code in the project
#![allow(dead_code)]

mod disc_reader;
mod path_simulator;
mod plotter;

use path_simulator::{Launch, SimControls, SpinDirection};
use std::path::Path;

fn main() {
    let discs_filename = Path::new("./data/discs.json");

    let discs = match disc_reader::discs_from_filename(discs_filename) {
        Ok(val) => val,
        Err(msg) => panic!(
            "Could not load discs from {}: {}",
            std::fs::canonicalize(discs_filename)
                .and_then(|p| {
                    p.to_str()
                        .map(str::to_string)
                        .ok_or_else(|| std::io::Error::new(std::io::ErrorKind::Other, "Could not create absolute path"))
                })
                .unwrap(),
            msg
        ),
    };

    let wraith = discs
        .iter()
        .find(|disc| disc.name == "Wraith")
        .expect("Could not find disc");

    let initial_trajectory = Launch {
        speed: 26.8,                             // m/s
        spin: 32.2 * 2.0 * std::f64::consts::PI, // rad/s
        spindir: SpinDirection::Clockwise,
        launch_angle: 0.1, // rad
        nose_angle: 0.1,   // rad
        roll_angle: -0.1,  // rad
        height: 1.5,       // meters (1.5 by default probably)
    };

    let controls = SimControls {
        dt: 0.1,
        max_time: 20.0,
    };

    let flight_path = path_simulator::simulate(wraith, &initial_trajectory, &controls);

    dbg!(&flight_path);

    let output_filename = Path::new("./flight_plot.bmp");
    plotter::plot(&flight_path, output_filename).expect("Failed to plot");
    println!("Plot saved to {}", output_filename.to_str().unwrap());
}
