use crate::path_simulator::coords;
use std::error::Error;
use std::path::Path;

use plotters::prelude::*;

pub fn plot(flight_path: &[coords::Ground], filename: &Path) -> Result<(), Box<dyn Error>> {
    let root = BitMapBackend::new(filename, (640, 480)).into_drawing_area();
    root.fill(&WHITE)?;

    let mut chart = ChartBuilder::on(&root)
        .caption("Flight path from above", ("sans-serif", 50).into_font())
        .margin(5)
        .x_label_area_size(30)
        .y_label_area_size(30)
        .build_cartesian_2d(-100f32..100f32, -1f32..500f32)?;

    chart.configure_mesh().draw()?;

    chart
        .draw_series(LineSeries::new(
            //(-50..=50).map(|x| x as f32 / 50.0).map(|x| (x, x * x)),
            flight_path
                .iter()
                .map(|coords| (coords.pos.x as f32, coords.pos.y as f32)),
            &BLUE,
        ))?
        .label("Flight path");
    //.legend(|(x, y)| PathElement::new(vec![(x, y), (x + 20, y)], &RED));

    chart
        .configure_series_labels()
        .background_style(&WHITE.mix(0.8))
        .border_style(&BLACK)
        .draw()?;

    root.present()?;

    Ok(())
}
