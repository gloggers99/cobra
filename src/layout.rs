use crate::cobra::{Cobra};
use crate::window::CobraWindow;

use x11::xlib::{XDefaultScreenOfDisplay, XHeightOfScreen, XSync, XWidthOfScreen};

pub trait Layout {
    fn arrange(&self, cobra: &Cobra, windows: &Vec<CobraWindow>);
}

pub struct Tiling {} impl Layout for Tiling {
    fn arrange(&self, cobra: &Cobra, windows: &Vec<CobraWindow>) {
        // If we don't have any windows we don't need to do anything!
        if windows.is_empty() {
            return;
        }

        let screen = unsafe { XDefaultScreenOfDisplay(cobra.display) };
        let screen_width = unsafe { XWidthOfScreen(screen) };
        let screen_height = unsafe { XHeightOfScreen(screen) };

        if windows.len() == 1 {
            windows[0].move_resize(
                cobra,
                0,
                0,
                screen_width as u32,
                screen_height as u32
            );
        } else {
            windows[0].move_resize(
                cobra,
                0,
                0,
                (screen_width / 2) as u32,
                screen_height as u32
            );

            for (i, win) in windows.iter().enumerate().skip(1) {
                let stack_count = windows.len() - 1;
                let stack_x = screen_width / 2;
                let stack_y = (screen_height / stack_count as i32) * (i as i32 - 1);
                let stack_w = screen_width / 2;
                let stack_h = screen_height / stack_count as i32;

                win.move_resize(
                    cobra,
                    stack_x,
                    stack_y,
                    stack_w as u32,
                    stack_h as u32
                );
            }
        }

        unsafe { XSync(cobra.display, 0) };
    }
}
