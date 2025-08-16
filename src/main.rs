mod cobra;
mod config;
mod event;
mod utility;

use std::error::Error;
use cobra::Cobra;

use crate::config::Config;

fn main() -> Result<(), Box<dyn Error>> {
    let mut cobra = Cobra::init(Config::default())?;
    cobra.run()?;

    Ok(())
}
