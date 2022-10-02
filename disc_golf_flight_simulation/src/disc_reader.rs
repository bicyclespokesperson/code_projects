use std::error::Error;
use std::fs::File;
use std::io::BufReader;
use std::path::Path;

extern crate serde_json;

use crate::path_simulator::Disc;

use serde_derive::Deserialize;
use serde_derive::Serialize;

// Generated with: https://transform.tools/json-to-rust-serde
#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct DiscJS {
    pub name: String,
    pub aoarange: Vec<f64>,
    pub cl: Vec<f64>,
    pub cd: Vec<f64>,
    pub cm: Vec<f64>,
    pub jxy: f64,
    pub jz: f64,
    pub diam: f64,
}

pub fn discs_from_filename(filename: &Path) -> Result<Vec<Disc>, Box<dyn Error>> {
    let file = File::open(filename)?;
    let reader = BufReader::new(file);
    let discs: Vec<DiscJS> = serde_json::from_reader(reader)?;

    let disc_mass = 0.175; // kg

    Ok(discs
        .into_iter()
        .map(|disc| {
            Disc::new_from_lists(
                disc.name,
                disc.jxy,
                disc.jz,
                disc.diam,
                disc_mass,
                &disc.aoarange,
                &disc.cl,
                &disc.cd,
                &disc.cm,
            )
        })
        .collect())
}
