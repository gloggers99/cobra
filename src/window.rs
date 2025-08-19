use std::ffi::{c_int, c_uint};

use x11::xlib::{ButtonPressMask, IsViewable, PropertyChangeMask, StructureNotifyMask, SubstructureNotifyMask, SubstructureRedirectMask, True, Window, XAddToSaveSet, XBlackPixel, XBlackPixelOfScreen, XCreateSimpleWindow, XDefaultScreen, XDefaultScreenOfDisplay, XGetWindowAttributes, XMoveResizeWindow, XReparentWindow, XResizeWindow, XSelectInput, XSetWindowBorderWidth, XWhitePixel, XWindowAttributes};

use crate::cobra::Cobra;

/// Represents a framed, reparented window.
#[derive(Clone, Debug)]
pub struct CobraWindow {
    frame: Window,
    child: Window
} impl CobraWindow {
    pub fn frame(&self) -> Window {
        self.frame
    }

    pub fn child(&self) -> Window {
        self.child
    }

    pub fn new(cobra: &Cobra, child: Window) -> Option<Self> {
        let mut window_attributes: XWindowAttributes = unsafe { std::mem::zeroed() };
        unsafe { XGetWindowAttributes(cobra.display, child, &mut window_attributes); }

        // Exit if the window is marked as "override_redirect" (polybar and things that don't want
        // to be managed).
        if window_attributes.override_redirect == True || window_attributes.map_state == IsViewable {
            return None
        }

        //unsafe { XResizeWindow(cobra.display, child, (window_attributes.width - border_width as i32) as c_uint, (window_attributes.height - border_width as i32) as c_uint); }
        unsafe { XSetWindowBorderWidth(cobra.display, child, 0); }

        let frame_w = (window_attributes.width as c_uint).saturating_sub(cobra.config.border_size * 2);
        let frame_h = (window_attributes.height as c_uint).saturating_sub(cobra.config.border_size * 2);

        let frame = unsafe {
            XCreateSimpleWindow(
                cobra.display,
                cobra.root,
                window_attributes.x,
                window_attributes.y,
                frame_w,
                frame_h,
                cobra.config.border_size,
                cobra.config.normal_border_color,
                XBlackPixelOfScreen(XDefaultScreenOfDisplay(cobra.display))
            )
        };

        // Restore window if we crash.
        unsafe { XAddToSaveSet(cobra.display, child); }
        // Stick window into the frame.
        unsafe { XReparentWindow(cobra.display, child, frame, 0, 0); }
        // Send events to the window and frame properly
        unsafe { XSelectInput(cobra.display, child, StructureNotifyMask | PropertyChangeMask | ButtonPressMask); }
        unsafe { XSelectInput(cobra.display, frame, SubstructureRedirectMask | SubstructureNotifyMask | ButtonPressMask); }

        Some(CobraWindow {
            frame,
            child
        })
    }

    pub fn move_resize(&self, cobra: &Cobra, x: c_int, y: c_int, width: c_uint, height: c_uint) {
        unsafe {
            // Move the frame window.
            XMoveResizeWindow(
                cobra.display,
                self.frame,
                x,
                y,
                width.saturating_sub(cobra.config.border_size * 2),
                height.saturating_sub(cobra.config.border_size * 2)
            );

            XMoveResizeWindow(
                cobra.display,
                self.child,
                0,
                0,
                width.saturating_sub(cobra.config.border_size * 2),
                height.saturating_sub(cobra.config.border_size * 2)
            );
        }
    }
}

