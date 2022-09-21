extern crate nalgebra_glm as glm;

use glm::Vec3;

pub struct Disc {
    speed: f64,
    glide: f64,
    turn: f64,
    fade: f64,
}

pub struct Launch {
    initial_position: Vec3,
    initial_trajectory: Vec3,
    initial_normal: Vec3,
}

pub fn simulate(
    disc: &Disc,
    initial_trajectory: &Launch
) -> Vec<Vec3> {
    vec![Vec3::new(1.0, 1.0, 1.2)]
}
