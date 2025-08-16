use crate::cobra::Cobra;

use std::error::Error;
use std::{ffi::c_uint, process::Command};
use std::mem;

use x11::xlib::{ButtonPressMask, IsViewable, PropertyChangeMask, StructureNotifyMask, SubstructureNotifyMask, SubstructureRedirectMask, True, Window, XAddToSaveSet, XCreateSimpleWindow, XDefaultScreen, XGetWindowAttributes, XReparentWindow, XResizeWindow, XSelectInput, XSetWindowBorderWidth, XWhitePixel, XWindowAttributes, XKeyPressedEvent};

/// Take window and reparent it into a frame.
pub fn frame_window(cobra: &Cobra, window: Window) -> Option<(Window /* Frame */, Window /* Original Window */)> {
    let border_width: c_uint = 3;

    let mut window_attributes: XWindowAttributes = unsafe { mem::zeroed() };
    unsafe { XGetWindowAttributes(cobra.display, window, &mut window_attributes); }
    unsafe { XResizeWindow(cobra.display, window, (window_attributes.width - border_width as i32) as c_uint, (window_attributes.height - border_width as i32) as c_uint); }
    unsafe { XSetWindowBorderWidth(cobra.display, window, 0); }

    // Exit if the window is marked as "override_redirect" (polybar and things that don't want
    // to be managed).
    if window_attributes.override_redirect == True || window_attributes.map_state == IsViewable {
        return None
    }

    let frame = unsafe { XCreateSimpleWindow(
        cobra.display,
        cobra.root,
        window_attributes.x,
        window_attributes.y,
        window_attributes.width as c_uint - border_width * 2,
        window_attributes.height as c_uint - border_width * 2,
        border_width,
        0xFF0000,
        XWhitePixel(cobra.display, XDefaultScreen(cobra.display))
    ) };

    // Restore window if we crash.
    unsafe { XAddToSaveSet(cobra.display, window); }
    // Stick window into the frame.
    unsafe { XReparentWindow(cobra.display, window, frame, 0, 0); }
    // Send events to the window and frame properly
    unsafe { XSelectInput(cobra.display, window, StructureNotifyMask | PropertyChangeMask); }
    unsafe { XSelectInput(cobra.display, frame, SubstructureRedirectMask | SubstructureNotifyMask | ButtonPressMask); }

    Some((frame, window))
}

