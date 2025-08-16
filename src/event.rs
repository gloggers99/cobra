use crate::cobra::Cobra;
use crate::utility::frame_window;

use std::error::Error;
use std::ffi::c_uint;
use std::mem;

use x11::xlib::{self, XKeyPressedEvent, XKeysymToKeycode};
use x11::xlib::{Window, XConfigureRequestEvent, XMapRequestEvent, XUnmapEvent};
use x11::xlib::{XConfigureWindow, XDestroyWindow, XEvent, XMapWindow, XRemoveFromSaveSet, XReparentWindow, XWindowChanges};

/// Map XEvent to a rust enum variant.
pub enum Event {
    ConfigureRequest(XConfigureRequestEvent),
    MapRequest(XMapRequestEvent),
    Unmap(XUnmapEvent),
    KeyPressed(XKeyPressedEvent),
    None(XEvent)
}

impl From<XEvent> for Event {
    fn from(event: XEvent) -> Self {
        match event.get_type() {
            xlib::ConfigureRequest => Self::ConfigureRequest(unsafe { event.configure_request }),
            xlib::MapRequest => Self::MapRequest(unsafe { event.map_request }),
            xlib::UnmapNotify => Self::Unmap(unsafe { event.unmap }),
            xlib::KeyPress => Self::KeyPressed(unsafe { event.key }),
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
        if let Some((frame, window)) = frame_window(cobra, event.window) {
            unsafe { XMapWindow(cobra.display, frame); }
            unsafe { XMapWindow(cobra.display, window); }

            cobra.windows.insert(frame, window);
        }

        Ok(())
    }

    /// When a window unmaps itself we will end up here. This is not a request event but a notify
    /// event so we do not need to "permit" the action. We do need to remove the frame/window from
    /// the windows map.
    pub fn unmap(cobra: &mut Cobra, event: XUnmapEvent) -> Result<(), Box<dyn Error>> {
        let frames_to_remove: Vec<(Window, Window)> = cobra.windows
            .iter()
            .filter(|&(_, window)| *window == event.window)
            .map(|(&frame, &client)| (frame, client))
            .collect();

        for (frame, client) in frames_to_remove {
            unsafe {
                XReparentWindow(cobra.display, client, cobra.root, 0, 0);
                XRemoveFromSaveSet(cobra.display, client);
                XDestroyWindow(cobra.display, frame);
            }

            cobra.windows.remove(&frame);
        }

        Ok(())
    }

    pub fn keypressed(cobra: &mut Cobra, event: XKeyPressedEvent) -> Result<(), Box<dyn Error>> {
        for (key, action) in &cobra.config.keys {
            if unsafe { XKeysymToKeycode(cobra.display, key.key as u64) } == event.keycode as u8 {
                action.action(&event)?;
            }
        }

        Ok(())
    }
}
