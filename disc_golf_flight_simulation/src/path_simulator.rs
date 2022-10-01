extern crate nalgebra_glm as glm;

use glm::Vec3;

pub fn simulate(_disc: &Disc, _initial_trajectory: &Launch) -> Vec<Vec3> {
    vec![Vec3::new(1.0, 1.0, 1.2)]
}

fn update_throw(_disc: &Disc, _launch: &Launch) {}

pub struct Disc {
    name: String,
    props: Vec<AeroProps>, // Should be sorted by aoa ascending
    jxy: f64,
    jz: f64,
    diam: f64,
    mass: f64, // in grams
}

// clockwise=>1, counterclockwise=>-1. Will probably want a match statement on this at some point
enum SpinDirection {
    Clockwise,
    Counterclockwise,
}

// Initial conditions for the throw
pub struct Launch {
    speed: f64, // m/s
    spin: f64,  // rad/s
    spindir: SpinDirection,
    launch_angle: f64, // rad
    nose_angle: f64,   // rad
    roll_angle: f64,   // rad
}

pub struct AeroProps {
    angle_of_attack: f64,
    cl: f64,
    cd: f64,
    cm: f64,
}

impl Disc {
    pub fn new(
        name: String,
        props: Vec<AeroProps>,
        jxy: f64,
        jz: f64,
        diam: f64,
        mass: f64,
    ) -> Disc {
        Disc {
            name,
            props,
            jxy,
            jz,
            diam,
            mass,
        }
    }

    pub fn new_from_lists(
        name: String,
        jxy: f64,
        jz: f64,
        diam: f64,
        mass: f64,
        angles_of_attack: &[f64],
        cl: &[f64],
        cd: &[f64],
        cm: &[f64],
    ) -> Disc {
        assert!(
            angles_of_attack.len() == cl.len()
                && angles_of_attack.len() == cd.len()
                && angles_of_attack.len() == cm.len()
        );

        let mut props: Vec<AeroProps> = Vec::new();
        for i in 0..angles_of_attack.len() {
            props.push(AeroProps {
                angle_of_attack: angles_of_attack[i],
                cl: cl[i],
                cd: cd[i],
                cm: cm[i],
            });
        }

        Disc {
            name,
            props,
            jxy,
            jz,
            diam,
            mass,
        }
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

    //TODO: Is this the right way to write the key_fn signature?
    fn interp(&self, angle_of_attack: f64, key_fn: fn(&AeroProps) -> f64) -> f64 {
        debug_assert!(
            !self.props.is_empty()
                && angle_of_attack > self.props[0].angle_of_attack
                && angle_of_attack < self.props.last().unwrap().angle_of_attack
        ); // angle of attack must be within the bounds of the array

        let location = self
            .props
            .partition_point(|prop| prop.angle_of_attack < angle_of_attack);

        debug_assert!(location > 0 && location < self.props.len() - 1); // Only values within our know angles of attack are allows

        let lower_item = &self.props[location - 1];
        let higher_item = &self.props[location];
        let lower_aoa = lower_item.angle_of_attack;
        let higher_aoa = higher_item.angle_of_attack;
        let lower_prop = key_fn(lower_item);
        let higher_prop = key_fn(higher_item);

        lower_prop
            + (angle_of_attack - lower_aoa) * (higher_prop - lower_prop) / (higher_aoa - lower_aoa)
    }
}

// Transformations between coordinate systems
mod transforms {

    extern crate nalgebra_glm as glm;
    use glm::{Mat3x3, Vec3};

    fn gd(angles: Vec3) -> Mat3x3 {
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

    fn dg(angles: Vec3) -> Mat3x3 {
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

    fn ds(beta: f32) -> Mat3x3 {
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

    fn sd(beta: f32) -> Mat3x3 {
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

    fn sw(alpha: f32) -> Mat3x3 {
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

    fn ws(alpha: f32) -> Mat3x3 {
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
}
