use std::collections::HashMap;
use std::ffi::c_uint;
use std::error::Error;
use std::process::Command;

use x11::xlib::{self, XKeyPressedEvent, XKillClient};
use x11::keysym;

use crate::cobra::Cobra;

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

pub trait KeybindAction {
    fn action(&self, cobra: &Cobra, event: &XKeyPressedEvent) -> Result<(), Box<dyn Error>>;
}

/// Spawn a command.
///
/// This is for spawning programs like a terminal or dmenu/rofi.
pub struct Spawn {
    command: String
} impl KeybindAction for Spawn {
    fn action(&self, _: &Cobra, _: &XKeyPressedEvent) -> Result<(), Box<dyn Error>> {
        let _ = Command::new("sh").arg("-c").arg(&self.command).spawn()?;
        Ok(())
    }
}

/// This works by returning an explicit quit error.
pub struct Quit {} impl KeybindAction for Quit {
    fn action(&self, _: &Cobra, _: &XKeyPressedEvent) -> Result<(), Box<dyn Error>> {
        Err(String::from("Quit KeybindAction called."))?
    }
}

pub struct Kill {} impl KeybindAction for Kill {
    fn action(&self, cobra: &Cobra, event: &XKeyPressedEvent) -> Result<(), Box<dyn Error>> {
        if let Some(window) = &cobra.focused_window {
            unsafe { XKillClient(cobra.display, window.child()); }
        }

        Ok(())
    }
}

pub struct Config {
    pub terminal_command: String,
    pub launcher_command: String,

    pub border_size: c_uint,

    pub normal_border_color: u64,
    pub focused_border_color: u64,

    pub keymap: HashMap<Keybind, Box<dyn KeybindAction>>
} impl Default for Config {
    fn default() -> Self {
        let terminal_command = "ghostty".to_owned();
        let launcher_command = "rofi -show drun -show-icons".to_owned();

        Self {
            terminal_command: terminal_command.clone(),
            launcher_command: launcher_command.clone(),

            border_size: 2,

            normal_border_color: 0xffffff,
            focused_border_color: 0xfF0000,

            keymap: HashMap::from([
                (Keybind::new(xlib::Mod1Mask | xlib::ShiftMask, keysym::XK_Return), Box::new(Spawn { command: terminal_command }) as Box<dyn KeybindAction>),
                (Keybind::new(xlib::Mod1Mask,                   keysym::XK_p),      Box::new(Spawn { command: launcher_command }) as Box<dyn KeybindAction>),
                (Keybind::new(xlib::Mod1Mask | xlib::ShiftMask, keysym::XK_q),      Box::new(Quit {}) as Box<dyn KeybindAction>),
                (Keybind::new(xlib::Mod1Mask | xlib::ShiftMask, keysym::XK_c),      Box::new(Kill {}) as Box<dyn KeybindAction>)
            ])
        }
    }
}
