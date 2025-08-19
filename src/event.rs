use crate::cobra::{Cobra};
use crate::window::CobraWindow;

use std::error::Error;
use std::ffi::c_uint;
use std::mem;

use x11::xlib::{self, XButtonPressedEvent, XKeyPressedEvent, XKeysymToKeycode};
use x11::xlib::{XConfigureRequestEvent, XMapRequestEvent, XUnmapEvent};
use x11::xlib::{XConfigureWindow, XDestroyWindow, XEvent, XMapWindow, XRemoveFromSaveSet, XReparentWindow, XWindowChanges};

/// Map XEvent to a rust enum variant.
pub enum Event {
    ConfigureRequest(XConfigureRequestEvent),
    MapRequest(XMapRequestEvent),
    Unmap(XUnmapEvent),
    KeyPressed(XKeyPressedEvent),
    ButtonPressed(XButtonPressedEvent),
    None(XEvent)
}

impl From<XEvent> for Event {
    fn from(event: XEvent) -> Self {
        match event.get_type() {
            xlib::ConfigureRequest => Self::ConfigureRequest(unsafe { event.configure_request }),
            xlib::MapRequest => Self::MapRequest(unsafe { event.map_request }),
            xlib::UnmapNotify => Self::Unmap(unsafe { event.unmap }),
            xlib::KeyPress => Self::KeyPressed(unsafe { event.key }),
            xlib::ButtonPress => Self::ButtonPressed(unsafe { event.button }),
            _ => Self::None(event)
        }
    }
}

impl Event {
    /// Setup a window with its requested defaults.
    pub fn configure_request(cobra: &mut Cobra, event: XConfigureRequestEvent) -> Result<(), Box<dyn Error>> {
        let mut window_changes: XWindowChanges = unsafe { mem::zeroed() };
        window_changes.x = event.x;
        window_changes.y = event.y;
        window_changes.width = event.width;
        window_changes.height = event.height;
        window_changes.border_width = event.border_width;
        window_changes.sibling = event.above;
        window_changes.stack_mode = event.detail;

        unsafe { XConfigureWindow(cobra.display, event.window, event.value_mask as c_uint, &mut window_changes); }

        Ok(())
    }

    /// Here we will "map" the window onto the screen when requested.
    pub fn map_request(cobra: &mut Cobra, event: XMapRequestEvent) -> Result<(), Box<dyn Error>> {
        if let Some(window) = CobraWindow::new(cobra, event.window) {
            unsafe { XMapWindow(cobra.display, window.frame()); }
            unsafe { XMapWindow(cobra.display, window.child()); }


            cobra.focus_window(window.clone());

            cobra.windows.push(window);
        }

        cobra.layout.arrange(cobra, &cobra.windows);

        Ok(())
    }

    /// When a window unmaps itself we will end up here. This is not a request event but a notify
    /// event so we do not need to "permit" the action. We do need to remove the frame/window from
    /// the windows map.
    pub fn unmap(cobra: &mut Cobra, event: XUnmapEvent) -> Result<(), Box<dyn Error>> {
        // Find the index of the window to remove (if it exists)
        let window_index = cobra.windows.iter()
            .position(|window| window.child() == event.window);

        if let Some(index) = window_index {
            let window = &cobra.windows[index];
            let frame = window.frame();
            let client = window.child();

            unsafe {
                XReparentWindow(cobra.display, client, cobra.root, 0, 0);
                XRemoveFromSaveSet(cobra.display, client);
                XDestroyWindow(cobra.display, frame);
            }

            cobra.focused_window = None;

            // Remove the window at the found index
            cobra.windows.remove(index);
        }

        cobra.layout.arrange(cobra, &cobra.windows);

        Ok(())
    }

    pub fn keypressed(cobra: &mut Cobra, event: XKeyPressedEvent) -> Result<(), Box<dyn Error>> {
        for (key, action) in &cobra.config.keymap {
            if unsafe { XKeysymToKeycode(cobra.display, key.key as u64) } == event.keycode as u8 {
                action.action(cobra, &event)?;
            }
        }

        Ok(())
    }

    pub fn buttonpressed(cobra: &mut Cobra, event: XButtonPressedEvent) -> Result<(), Box<dyn Error>> {
        println!("{:?},\n Looking for {}", cobra.windows, event.subwindow);
        let window = cobra.windows.iter().find(|cobra_window|
            cobra_window.frame() == event.subwindow || cobra_window.child() == event.subwindow);
        println!("Searching for window");
        if let Some(window) = window {
            println!("Found window");
            cobra.focus_window(window.clone());
        }

        Ok(())
    }
}
