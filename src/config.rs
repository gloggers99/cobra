use std::collections::HashMap;
use std::ffi::c_uint;
use std::error::Error;
use std::process::Command;

use x11::xlib::{self, XKeyPressedEvent};
use x11::keysym;

pub trait KeybindAction {
    fn action(&self, event: &XKeyPressedEvent) -> Result<(), Box<dyn Error>>;
}

#[derive(PartialEq, Eq, Hash)]
pub struct Keybind {
    pub modifier: u32,
    pub key: u32
} impl Keybind {
    pub fn new(modifier: u32, key: u32) -> Self {
        Self {
            modifier,
            key
        }
    }
}

/// Spawn a command
///
/// This is for spawning programs like a terminal or dmenu/rofi
pub struct Spawn {
    command: String
} impl KeybindAction for Spawn {
    fn action(&self, _: &XKeyPressedEvent) -> Result<(), Box<dyn Error>> {
        let _ = Command::new("sh").arg("-c").arg(&self.command).spawn()?;
        Ok(())
    }
}

pub struct Config {
    pub terminal_command: String,
    pub launcher_command: String,

    pub border_size: c_uint,

    pub keys: HashMap<Keybind, Box<dyn KeybindAction>>
} impl Default for Config {
    fn default() -> Self {
        let terminal_command = "ghostty".to_owned();
        let launcher_command = "rofi -show drun -show-icons".to_owned();

        Self {
            terminal_command: terminal_command.clone(),
            launcher_command: launcher_command.clone(),

            border_size: 2,
            keys: HashMap::from([
                (Keybind::new(xlib::Mod1Mask | xlib::ShiftMask, keysym::XK_Return), Box::new(Spawn { command: terminal_command }) as Box<dyn KeybindAction>),
                (Keybind::new(xlib::Mod1Mask,                   keysym::XK_p),      Box::new(Spawn { command: launcher_command }) as Box<dyn KeybindAction>)
            ])
        }
    }
}
