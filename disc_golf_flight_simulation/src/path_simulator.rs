extern crate nalgebra_glm as glm;

use glm::{Mat3x3, Vec3};

struct AeroProps {
    angle_of_attack: f64,
    cl: f64,
    cd: f64,
    cm: f64,
}

pub struct Disc {
    name: String,
    props: Vec<AeroProps>, // Should be sorted by aoa ascending
    jxy: f64,
    jz: f64,
    diam: f64,
}

impl Disc {
    fn interp(&self, _angle_of_attack: f64, key_fn: fn(&AeroProps) -> f64) -> f64 {
        // TODO: impl using binary search (probably a vector builtin for this)
        //       might need a key fn to determine which property to use
        key_fn(&self.props[0])
    }

    pub fn get_cl(&self, aoa: f64) -> f64 {
        self.interp(aoa, |arg: &AeroProps| arg.cl)
    }

    pub fn get_cd(&self, aoa: f64) -> f64 {
        self.interp(aoa, |arg: &AeroProps| arg.cd)
    }

    pub fn get_cm(&self, aoa: f64) -> f64 {
        self.interp(aoa, |arg: &AeroProps| arg.cm)
    }
}

pub struct Launch {
    initial_position: Vec3,
    initial_trajectory: Vec3,
    initial_normal: Vec3,
}

pub fn simulate(_disc: &Disc, _initial_trajectory: &Launch) -> Vec<Vec3> {
    vec![Vec3::new(1.0, 1.0, 1.2)]
}

// Transformations between coordinates
// TODO: Namespace this (however rust does that, crates?) and get rid of the t_ prefix

fn t_gd(angles: Vec3) -> Mat3x3 {
    let phi = angles[0];
    let theta = angles[1];
    let psi = angles[2];

    //TODO: Are numpy arrays/matrices rowwise? What about rust-glm matrices? Might need to transform this if they are not the same
    Mat3x3::new(
        theta.cos() * psi.cos(),
        phi.sin() * theta.sin() * psi.cos() - phi.cos() * psi.sin(),
        phi.cos() * theta.sin() * psi.cos() + phi.sin() * psi.sin(),
        theta.cos() * psi.sin(),
        phi.sin() * theta.sin() * psi.sin() + phi.cos() * psi.cos(),
        phi.cos() * theta.sin() * psi.sin() - phi.sin() * psi.cos(),
        -theta.sin(),
        phi.sin() * theta.cos(),
        phi.cos() * theta.cos(),
    )
}

fn t_dg(angles: Vec3) -> Mat3x3 {
    let phi = angles[0];
    let theta = angles[1];
    let psi = angles[2];
    Mat3x3::new(
        theta.cos() * psi.cos(),
        theta.cos() * psi.sin(),
        -theta.sin(),
        phi.sin() * theta.sin() * psi.cos() - phi.cos() * psi.sin(),
        phi.sin() * theta.sin() * psi.sin() + phi.cos() * psi.cos(),
        phi.sin() * theta.cos(),
        phi.cos() * theta.sin() * psi.cos() + phi.sin() * psi.sin(),
        phi.cos() * theta.sin() * psi.sin() - phi.sin() * psi.cos(),
        phi.cos() * theta.cos(),
    )
}

fn t_ds(beta: f32) -> Mat3x3 {
    Mat3x3::new(
        beta.cos(),
        -beta.sin(),
        0.0,
        beta.sin(),
        beta.cos(),
        0.0,
        0.0,
        0.0,
        1.0,
    )
}

fn t_sd(beta: f32) -> Mat3x3 {
    Mat3x3::new(
        beta.cos(),
        beta.sin(),
        0.0,
        -beta.sin(),
        beta.cos(),
        0.0,
        0.0,
        0.0,
        1.0,
    )
}

fn t_sw(alpha: f32) -> Mat3x3 {
    Mat3x3::new(
        alpha.cos(),
        0.0,
        -alpha.sin(),
        0.0,
        1.0,
        0.0,
        alpha.sin(),
        0.0,
        alpha.cos(),
    )
}

fn t_ws(alpha: f32) -> Mat3x3 {
    Mat3x3::new(
        alpha.cos(),
        0.0,
        alpha.sin(),
        0.0,
        1.0,
        0.0,
        -alpha.sin(),
        0.0,
        alpha.cos(),
    )
}
