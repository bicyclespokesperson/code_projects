extern crate nalgebra_glm as glm;
extern crate serde_json;

use glm::DVec3;

pub struct Disc {
    pub name: String,
    pub props: Vec<AeroProps>, // Should be sorted by aoa ascending
    pub jxy: f64,
    pub jz: f64,
    pub diam: f64,
    pub mass: f64, // in grams
}

// Initial conditions for the throw
pub struct Launch {
    pub speed: f64, // m/s
    pub spin: f64,  // rad/s
    pub spindir: SpinDirection,
    pub launch_angle: f64, // rad
    pub nose_angle: f64,   // rad
    pub roll_angle: f64,   // rad
    pub height: f64,       // meters (1.5 by default probably)
}

pub struct SimControls {
    pub dt: f64,       // seconds
    pub max_time: f64, // seconds
}

pub enum SpinDirection {
    Clockwise,
    Counterclockwise,
}

pub struct AeroProps {
    pub angle_of_attack: f64,
    pub cl: f64,
    pub cd: f64,
    pub cm: f64,
}

pub mod coords {

    extern crate nalgebra_glm as glm;
    use glm::DVec3;

    #[derive(Default, Debug)]
    pub struct Ground {
        pub pos: DVec3, // position in m
        pub vel: DVec3, // velocity in m/s
        pub acl: DVec3, // acceleration in m/s^2
        pub ori: DVec3, // rol, pitch, yaw in rad
        pub rot: DVec3, // roll, pitch, yaw rate in rad/s
        pub time: f64,  // time in seconds
    }

    #[derive(Default)]
    pub struct Disc {
        pub acl: DVec3, // acceleration in m/s^2
        pub vel: DVec3, // velocity in m/s
        pub rot: DVec3, // roll, pitch, yaw rate in rad/s
    }

    #[derive(Default)]
    pub struct SideSlip {
        pub acl: DVec3, // acceleration in m/s^2
        pub vel: DVec3, // velocity in m/s
        pub rot: DVec3, // roll, pitch, yaw rate in rad/s
        pub beta: f64,
    }

    #[derive(Default)]
    pub struct Wind {
        pub acl: DVec3, // acceleration in m/s^2
        pub vel: DVec3, // velocity in m/s
        pub alpha: f64,
    }
}

#[derive(Default)]
struct SimStep {
    ground_coords: coords::Ground,
    disc_coords: coords::Disc,
    side_slip_coords: coords::SideSlip,
    wind_coords: coords::Wind,

    drag: f64,
    lift: f64,
    mom: f64,
}

pub fn simulate(disc: &Disc, initial_trajectory: &Launch, controls: &SimControls) -> Vec<coords::Ground> {
    const AIR_DENSITY: f64 = 1.18; // Air density, rho in orig code. In kg/m^3
    const G: f64 = 9.81;
    const GROUND_HEIGHT: f64 = 0.0;

    let ixy = disc.jxy * disc.mass; // Rotational moment of inertia of disc about roll axis in kg-m^2
    let iz = disc.jz * disc.mass; // Rotational moment of inertia of disc about spin axis in kg-m^2
    let area = std::f64::consts::PI * (0.5 * disc.diam).powi(2); // Planform area of disc in m^2
    let omega = initial_trajectory.spin
        * match initial_trajectory.spindir {
            SpinDirection::Clockwise => 1.0,
            SpinDirection::Counterclockwise => -1.0,
        }; // Assign spin direction
    let weight = G * disc.mass; // Gravitational force acting on the disc center of mass in N

    let mut steps: Vec<SimStep> = Vec::new();

    let mut initial_step = SimStep::default();

    //ori_g[step] = np.array([throw.roll_angle, throw.nose_angle, 0])
    //vel_g[step] = np.array([throw.speed*np.cos(throw.launch_angle), 0, throw.speed*np.sin(throw.launch_angle)])
    //launch_angle_d = np.matmul(t_gd(ori_g[step]), [0, throw.launch_angle, 0])
    //ori_g[step] += launch_angle_d
    initial_step.ground_coords.ori = DVec3::new(initial_trajectory.roll_angle, initial_trajectory.nose_angle, 0.0);
    initial_step.ground_coords.vel = DVec3::new(
        initial_trajectory.speed * initial_trajectory.launch_angle.cos(),
        0.0,
        initial_trajectory.speed * initial_trajectory.launch_angle.sin(),
    );
    let launch_angle_d =
        transforms::gd(initial_step.ground_coords.ori) * DVec3::new(0.0, initial_trajectory.launch_angle, 0.0);
    initial_step.ground_coords.ori += launch_angle_d;
    initial_step.ground_coords.pos = DVec3::new(0.0, 0.0, initial_trajectory.height);
    steps.push(initial_step);

    while steps.last().unwrap().ground_coords.pos.z > GROUND_HEIGHT {
        //if step >= maxSteps: # Safety valve in case the disc never returns to earth
        //  break

        let mut ii: u32 = 0;
        let mut step = SimStep::default();

        if steps.len() > 1 {
            //vel_g[step+1] = vel_g[step] + acl_g[step]*dt
            //pos_g[step+1] = pos_g[step] + vel_g[step]*dt + 0.5*acl_g[step]*dt**2
            //ori_g[step+1] = ori_g[step] + rot_g[step]*dt
            let prev = &steps.last().unwrap();
            step.ground_coords.vel = prev.ground_coords.vel + prev.ground_coords.acl * controls.dt;
            step.ground_coords.pos = prev.ground_coords.pos
                + prev.ground_coords.vel * controls.dt
                + 0.5 * prev.ground_coords.acl * controls.dt.powf(2.0);
            step.ground_coords.ori = prev.ground_coords.ori + prev.ground_coords.rot * controls.dt;
        }

        loop {
            //TODO: populate and add to steps vector

            // Transform ground velocity to wind coordinate system
            //vel_d[step] = np.matmul(T_gd(ori_g[step]), vel_g[step]); // Transform ground velocity to disc coordinate system
            //beta[step] = -np.arctan2(vel_d[step][1], vel_d[step][0]); // Calculate side slip angle
            //vel_s[step] = np.matmul(T_ds(beta[step]), vel_d[step]); // Transform velocity to zero side-slip coordinate system
            //alpha[step] = -np.arctan2(vel_s[step][2], vel_s[step][0]); // Calculate the angle of attack
            //vel_w[step] = np.matmul(T_sw(alpha[step]), vel_s[step]); // Transform velocity to wind coordinate system where aerodynamic calculations can be made
            step.disc_coords.vel = transforms::gd(step.ground_coords.ori) * step.ground_coords.vel;
            step.side_slip_coords.beta = -(step.disc_coords.vel.y.atan2(step.disc_coords.vel.x));
            step.side_slip_coords.vel = transforms::ds(step.side_slip_coords.beta) * step.disc_coords.vel;
            step.wind_coords.alpha = -(step.side_slip_coords.vel.z.atan2(step.side_slip_coords.vel.x));
            step.wind_coords.vel = transforms::sw(step.wind_coords.alpha) * step.side_slip_coords.vel;

            // Transform gravity loads to wind coordinate system
            //grav_d = np.matmul(T_gd(ori_g[step]), [0, 0, -weight]);
            //grav_s = np.matmul(T_ds(beta[step]), grav_d);
            //grav_w = np.matmul(T_sw(alpha[step]), grav_s);
            let grav_d = transforms::gd(step.ground_coords.ori) * DVec3::new(0.0, 0.0, G * weight);
            let grav_s = transforms::ds(step.side_slip_coords.beta) * grav_d;
            let grav_w = transforms::sw(step.wind_coords.alpha) * grav_s;

            // Calculate aerodynamic forces on the disc
            //drag[step] = 0.5*rho*(vel_w[step][0]**2)*area*disc.getCd(alpha[step]); // Calculate drag force in N
            //lift[step] = 0.5*rho*(vel_w[step][0]**2)*area*disc.getCl(alpha[step]); // Calculate lift force in N
            //mom[step] =  0.5*rho*(vel_w[step][0]**2)*area*diam*disc.getCm(alpha[step]); // Calculate pitching moment in N-m
            step.drag =
                0.5 * AIR_DENSITY * (step.wind_coords.vel.x.powf(2.0)) * area * disc.get_cl(step.wind_coords.alpha);
            step.lift =
                0.5 * AIR_DENSITY * (step.wind_coords.vel.x.powf(2.0)) * area * disc.get_cl(step.wind_coords.alpha);
            step.lift = 0.5
                * AIR_DENSITY
                * (step.wind_coords.vel.x.powf(2.0))
                * area
                * disc.diam
                * disc.get_cm(step.wind_coords.alpha);

            // Calculate body accelerations from second law and force balances
            //acl_w[step,0] = (-drag[step] + grav_w[0]) / mass; // Calculate deceleration due to drag
            //acl_w[step,2] = (lift[step] + grav_w[2]) / mass; // Calculate acceleration due to lift
            //acl_w[step,1] = grav_w[1] / mass; // Calculate acceleration due to side loading (just gravity)
            //rot_s[step,0] = -mom[step]/(omega*(ixy - iz)); // Calculate roll rate from pitching moment
            step.wind_coords.acl.x = (-step.drag + grav_w.x) / disc.mass;
            step.wind_coords.acl.z = (step.lift + grav_w.z) / disc.mass;
            step.wind_coords.acl.y = grav_w.y / disc.mass;
            step.side_slip_coords.rot.x = -step.mom / omega * (ixy - iz);

            // Tranform disc acceleration to ground coordinate system
            //acl_s[step] = np.matmul(T_ws(alpha[step]), acl_w[step]);
            //acl_d[step] = np.matmul(T_sd(beta[step]), acl_s[step]);
            //acl_g[step] = np.matmul(T_dg(ori_g[step]), acl_d[step]);
            step.side_slip_coords.acl = transforms::ws(step.wind_coords.alpha) * step.wind_coords.acl;
            step.disc_coords.acl = transforms::sd(step.side_slip_coords.beta) * step.side_slip_coords.acl;
            step.ground_coords.acl = transforms::dg(step.ground_coords.ori) * step.disc_coords.acl;

            // Transform roll rate from zero side-slip to ground coordinate system
            //rot_d[step] = np.matmul(T_sd(beta[step]), rot_s[step]);
            //rot_g[step] = np.matmul(T_dg(ori_g[step]), rot_d[step]);
            step.disc_coords.rot = transforms::sd(step.side_slip_coords.beta) * step.side_slip_coords.rot;
            step.ground_coords.rot = transforms::dg(step.ground_coords.ori) * step.disc_coords.rot;

            //TODO: clean up this logic
            // Perform one inner iteration to refine speed and position vectors
            if steps.len() == 1 {
                // Do not run inner iterations for initial time step
                break;
            }
            if ii >= 1 {
                // Only run one inner iteration
                break;
            }

            let prev = &steps.last().unwrap();

            // Calculate average accelerations and rotation rates between current and previous time steps
            //avg_acl_g = (acl_g[step-1] + acl_g[step])/2;
            //avg_rot_g = (rot_g[step-1] + rot_g[step])/2;
            let avg_acl_g = (prev.ground_coords.acl + step.ground_coords.acl) / 2.0;
            let avg_rot_g = (prev.ground_coords.rot + step.ground_coords.rot) / 2.0;

            // Calculate new velocity, position and orientation for current time step
            //vel_g[step] = vel_g[step-1] + avg_acl_g*dt;
            //pos_g[step] = pos_g[step-1] + vel_g[step-1]*dt + 0.5*avg_acl_g*dt**2;
            //ori_g[step] = ori_g[step-1] + avg_rot_g*dt;
            step.ground_coords.vel = prev.ground_coords.vel + avg_acl_g * controls.dt;
            step.ground_coords.pos =
                prev.ground_coords.pos + prev.ground_coords.vel * controls.dt + 0.5 * avg_acl_g * controls.dt.powf(2.0);
            step.ground_coords.ori = prev.ground_coords.ori + avg_rot_g * controls.dt;

            ii += 1
        }

        //THIS IS MOVED TO THE TOP OF THE FUNCTION
        // Estimate disc velocity, position, and orientation at next time step
        //vel_g[step+1] = vel_g[step] + acl_g[step]*dt
        //pos_g[step+1] = pos_g[step] + vel_g[step]*dt + 0.5*acl_g[step]*dt**2
        //ori_g[step+1] = ori_g[step] + rot_g[step]*dt

        // Update simulation variables
        //t[step+1] = t[step] + dt
        //step += 1
        step.ground_coords.time = steps.last().unwrap().ground_coords.time + controls.dt;
        steps.push(step);
    }

    steps.into_iter().map(|step| step.ground_coords).collect()
}

impl Disc {
    pub fn new(name: String, props: Vec<AeroProps>, jxy: f64, jz: f64, diam: f64, mass: f64) -> Disc {
        Disc {
            name,
            props,
            jxy,
            jz,
            diam,
            mass,
        }
    }

    #[allow(clippy::too_many_arguments)]
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

        lower_prop + (angle_of_attack - lower_aoa) * (higher_prop - lower_prop) / (higher_aoa - lower_aoa)
    }
}

// Transformations between coordinate systems
mod transforms {

    extern crate nalgebra_glm as glm;
    use glm::{DMat3x3, DVec3};

    pub fn gd(angles: DVec3) -> DMat3x3 {
        let phi = angles[0];
        let theta = angles[1];
        let psi = angles[2];

        DMat3x3::new(
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

    pub fn dg(angles: DVec3) -> DMat3x3 {
        let phi = angles[0];
        let theta = angles[1];
        let psi = angles[2];
        DMat3x3::new(
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

    pub fn ds(beta: f64) -> DMat3x3 {
        DMat3x3::new(beta.cos(), -beta.sin(), 0.0, beta.sin(), beta.cos(), 0.0, 0.0, 0.0, 1.0)
    }

    pub fn sd(beta: f64) -> DMat3x3 {
        DMat3x3::new(beta.cos(), beta.sin(), 0.0, -beta.sin(), beta.cos(), 0.0, 0.0, 0.0, 1.0)
    }

    pub fn sw(alpha: f64) -> DMat3x3 {
        DMat3x3::new(
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

    pub fn ws(alpha: f64) -> DMat3x3 {
        DMat3x3::new(
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
