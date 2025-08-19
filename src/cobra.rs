use crate::event::{Event};
use crate::config::Config;
use crate::layout::{Layout, Tiling};
use crate::window::CobraWindow;

use x11::xlib::{BadAccess, ButtonPressMask, ButtonReleaseMask, CurrentTime, Display, GrabModeAsync, PointerMotionMask, RevertToParent, SubstructureNotifyMask, SubstructureRedirectMask, Window, XCloseDisplay, XDefaultRootWindow, XErrorEvent, XGrabButton, XGrabKey, XKeysymToKeycode, XNextEvent, XOpenDisplay, XRaiseWindow, XSelectInput, XSetErrorHandler, XSetInputFocus, XSetWindowBorder, XSync};

use std::error::Error;
use std::ffi::{c_int, c_uint};
use std::{mem, ptr};

/// Because of how rust interop works we have to define this here as an external function.
pub unsafe extern "C" fn xorg_error_handler(_display: *mut Display,
                                            error: *mut XErrorEvent) -> i32 {
    unsafe {
        if error.is_null() {
            panic!("xorg_error_handler: recieved null pointer? This should not happen.")
        }

        let error_code = (*error).error_code;
        let request_code = (*error).request_code;
        let minor_code = (*error).minor_code;

        if error_code == BadAccess {
            panic!("Another window manager is already running!");
        } else {
            eprintln!("An X11 error was triggered: error_code={}, request_code={}, minor_code={}", error_code, request_code, minor_code);
        }
    }

    0
}

pub struct Cobra {
    pub config: Config,

    pub display: *mut Display,
    /// Map window frames to windows
    pub root: Window,

    /// List of currently active windows.
    pub windows: Vec<CobraWindow>,
    /// Currently focused window
    pub focused_window: Option<CobraWindow>,

    pub layout: Box<dyn Layout>
} impl Drop for Cobra {
    /// Cleanup cobra window manager, close display, etc.
    fn drop(&mut self) {
        if !self.display.is_null() {
            unsafe { XCloseDisplay(self.display); }
        }
    }
} impl Cobra {
    /// Initialize cobra window manager.
    ///
    /// This will open the display and prepare to run cobra.
    pub fn init(config: Config) -> Result<Self, Box<dyn Error>> {
        unsafe { XSetErrorHandler(Some(xorg_error_handler)); }

        let display = unsafe { XOpenDisplay(ptr::null_mut()) };
        if display.is_null() {
            Err(String::from("Failed to open X display"))?
        }

        let root = unsafe { XDefaultRootWindow(display) };

        unsafe {
            // Take over X server redirects
            XSelectInput(display, root, SubstructureRedirectMask | SubstructureNotifyMask);
            XSync(display, false as c_int);
        }

        unsafe {
            XGrabButton(display, 1, 0, XDefaultRootWindow(display), true as c_int,
                             (ButtonPressMask|ButtonReleaseMask|PointerMotionMask) as c_uint, GrabModeAsync, GrabModeAsync,
                             0, 0);

            XGrabButton(display, 3, 0, XDefaultRootWindow(display), true as c_int,
                             (ButtonPressMask|ButtonReleaseMask|PointerMotionMask) as c_uint, GrabModeAsync, GrabModeAsync,
                             0, 0);
        }

        for (key, _) in &config.keymap {
            unsafe {
                XGrabKey(
                    display,
                    XKeysymToKeycode(display, key.key as u64) as c_int,
                    key.modifier,
                    root,
                    true as c_int,
                    GrabModeAsync,
                    GrabModeAsync
                );
            }
        }

        Ok(Cobra {
            config,
            display,
            root,
            windows: vec![],
            focused_window: None,
            layout: Box::new(Tiling {})
        })
    }

    /// Start the window manager loop. This will continue until the program is requested to end.
    pub fn run(&mut self) -> Result<(), Box<dyn Error>> {
        // Force all queued events into the X server.
        // For unsafe function calls place the semicolon after the function call.
        unsafe { XSync(self.display, false as c_int); }

        // For unsafe declarations put the semicolon AFTER the unsafe block ending instead.
        let mut event = unsafe { mem::zeroed() };
        loop {
            unsafe { XNextEvent(self.display, &mut event); }

            match Event::from(event) {
                Event::ConfigureRequest(configure_request) => Event::configure_request(self, configure_request)?,
                Event::MapRequest(map_request) => Event::map_request(self, map_request)?,
                Event::Unmap(unmap) => Event::unmap(self, unmap)?,
                Event::KeyPressed(keypressed) => Event::keypressed(self, keypressed)?,
                Event::ButtonPressed(buttonpressed) => Event::buttonpressed(self, buttonpressed)?,

                Event::None(generic_event) => println!("Unhandled X event: {}", generic_event.get_type())
            }
        }
    }

    /// Set a new window as the currently focused window.
    pub fn focus_window(&mut self, window: CobraWindow) {
        if let Some(focused_window) = &self.focused_window {
            unsafe { XSetWindowBorder(self.display, focused_window.frame(), self.config.normal_border_color); }
        }

        self.focused_window = Some(window);
        // Because we wrap this in an option we need to do this too.
        if let Some(focused_window) = &self.focused_window {
            unsafe { XSetWindowBorder(self.display, focused_window.frame(), self.config.focused_border_color); }
            unsafe { XSetInputFocus(self.display, focused_window.frame(), RevertToParent, CurrentTime); }
            unsafe { XRaiseWindow(self.display, focused_window.frame()); }
        }
    }
}
