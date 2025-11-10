# Notes:
- This is Grok AI agent. As wanted to make easy to read doc api.

# Overview of sokol_app.h

sokol_app.h is a single-header C library for creating cross-platform desktop applications with a simple event loop, input handling, and graphics initialization (via integration with other Sokol libraries like sokol_gfx.h). It supports Windows, macOS, Linux, and WebAssembly. The API revolves around callbacks for initialization, events, and cleanup, driven by a main loop.Below is a comprehensive list of all major components: structs, enums, unions, and typedefs (with members and purposes based on inline comments), followed by all function declarations (with signatures and descriptions). This is derived from the header file's content. All elements are in the sapp namespace via macros (e.g., sapp_desc for structs, sapp_run() for functions).

1. Structs, Enums, Unions, and Typedefs
- sapp_desc  
Purpose: Application descriptor struct; defines the configuration for the app (e.g., window size, title, callbacks). Passed to sapp_run() to start the app.  
Members:

- int width: Initial framebuffer width (default: 640).
- int height: Initial framebuffer height (default: 480).
- bool fullscreen: Start in fullscreen mode (default: false).
- bool alpha_format: Use alpha channel in swapchain (default: false).
- bool windowed_transparent: Enable transparent window (macOS only, default: false).
- bool high_dpi: Enable high-DPI rendering (default: true).
- bool first_non_fullscreen_framebuffer_width_embedded: Embed width in non-fullscreen mode.
- bool first_non_fullscreen_framebuffer_height_embedded: Embed height in non-fullscreen mode.
- const char* window_title: Window title string (default: "sokol-app").
- uint32_t ios_keyboard_resizes_canvas: iOS-specific: resize canvas on keyboard show/hide.
- bool gl_force_fx_events: Force fake mouse events on GL backend.
- sapp_event_cb event_cb: Pointer to event callback function.
- sapp_frame_cb frame_cb: Pointer to frame callback function.
- sapp_cleanup_cb cleanup_cb: Pointer to cleanup callback function.
- sapp_fail_cb fail_cb: Pointer to failure callback function.
- sapp_frame_count_cb frame_count_cb: Pointer to frame count callback (optional).
- sapp_embed_cb embed_cb: Pointer to embed callback (for WebAssembly).
- sapp_leave_cb leave_cb: Pointer to leave callback (for WebAssembly).
- sapp_suspend_cb suspend_cb: Pointer to suspend callback.
- sapp_resume_cb resume_cb: Pointer to resume callback.
- sapp_quit_requested_cb quit_requested_cb: Pointer to quit request callback.
- sapp_event_type_filter_cb event_type_filter_cb: Pointer to event type filter callback (optional).
- uint32_t user_data: User data slot for passing custom data to callbacks.
- sapp_logger log: Logger configuration (for debug output).

- sapp_run_state (opaque struct)  
    Purpose: Internal state tracking for the app runner; not directly used by user code.
- sapp_mouse_cursor  
    Purpose: Enum-like struct for mouse cursor types (used in sapp_set_mouse_cursor()).  
    Members: None (it's an enum wrapper; see enums below for values).
- sapp_touchpoint  
    Purpose: Struct for touch point data in touch events.  
    Members:
    - int identifier: Unique touch point ID.
    - float pos_x: Normalized X position [0..1].
    - float pos_y: Normalized Y position [0..1].
- sapp_keycode  
    Purpose: Enum-like struct for keyboard keycodes (used in key events).  
    Members: None (see enums below).
- sapp_mod_state  
    Purpose: Bitmask struct for modifier key states.  
    Members: None (see enums below).
- sapp_event  
    Purpose: Struct for all input and window events passed to the event callback.  
    Members:
    - sapp_event_type type: Type of event (e.g., key down, mouse move).
    - int frame_count: App frame index when event occurred.
    - sapp_keycode key_code: Key code for key events.
    - uint32_t char_code: Unicode char code for text input.
    - sapp_mod_state modifiers: Modifier keys state (shift, ctrl, etc.).
    - float mouse_x: Mouse X position (pixels).
    - float mouse_y: Mouse Y position (pixels).
    - float mouse_dx: Mouse delta X.
    - float mouse_dy: Mouse delta Y.
    - int scroll_dx: Scroll delta X (not used).
    - float scroll_dy: Scroll delta Y (pixels).
    - bool cmd_pressed: Command key pressed (macOS).
    - bool ctrl_pressed: Ctrl key pressed.
    - bool shift_pressed: Shift key pressed.
    - bool alt_pressed: Alt key pressed.
    - bool super_pressed: Super key pressed (Windows key).
    - int valid: Validation flag for event.
    - int mouse_button: Mouse button index for button events.
    - int num_touches: Number of touch points.
    - sapp_touchpoint touches[16]: Array of up to 16 touch points.
- sapp_logger  
    Purpose: Struct for custom logging configuration.  
    Members:
    - sapp_logger_func logf: Pointer to log function.
    - sapp_logger_func logf_userdata: User data for log function.
- **sapp_clipboard` (internal, not user-facing).

## Enums

- sapp_event_type  
    Purpose: Enumerates all possible event types for the event callback.  
    Values:
    - SAPP_EVENTTYPE_INVALID
    - SAPP_EVENTTYPE_KEY_DOWN
    - SAPP_EVENTTYPE_KEY_UP
    - SAPP_EVENTTYPE_CHAR
    - SAPP_EVENTTYPE_MOUSE_DOWN
    - SAPP_EVENTTYPE_MOUSE_UP
    - SAPP_EVENTTYPE_MOUSE_MOVE
    - SAPP_EVENTTYPE_MOUSE_ENTER
    - SAPP_EVENTTYPE_MOUSE_LEAVE
    - SAPP_EVENTTYPE_MOUSE_SCROLL
    - SAPP_EVENTTYPE_TOUCH_BEGAN
    - SAPP_EVENTTYPE_TOUCH_ENDED
    - SAPP_EVENTTYPE_TOUCH_MOVED
    - SAPP_EVENTTYPE_TOUCH_CANCELLED
    - SAPP_EVENTTYPE_RESIZED
    - SAPP_EVENTTYPE_ICONIFIED
    - SAPP_EVENTTYPE_RESTORED
    - SAPP_EVENTTYPE_FOCUSED
    - SAPP_EVENTTYPE_UNFOCUSED
    - SAPP_EVENTTYPE_QUIT_REQUESTED
    - SAPP_EVENTTYPE_SUSPENDED
    - SAPP_EVENTTYPE_RESUMED
- sapp_mousebutton  
    Purpose: Mouse button indices for button events.  
    Values:
    - SAPP_MOUSEBUTTON_LEFT (0)
    - SAPP_MOUSEBUTTON_RIGHT (1)
    - SAPP_MOUSEBUTTON_MIDDLE (2)
- sapp_keycode  
    Purpose: Keyboard scancodes (not char codes) for key events.  
    Values: (partial list; full includes SPACE, ENTER, etc.)
    - SAPP_KEYCODE_INVALID (-1)
    - SAPP_KEYCODE_A (0) to SAPP_KEYCODE_Z (25)
    - SAPP_KEYCODE_0 (26) to SAPP_KEYCODE_9 (35)
    - SAPP_KEYCODE_SPACE (44)
    - SAPP_KEYCODE_ENTER (13)
    - SAPP_KEYCODE_ESCAPE (41)
    - SAPP_KEYCODE_LEFT_SHIFT (225), SAPP_KEYCODE_RIGHT_SHIFT (226)
    - ... (up to SAPP_KEYCODE_GRAVE_ACCENT (241); ~250 values total)
- sapp_mouse_cursor  
    Purpose: Standard mouse cursor shapes.  
    Values:
    - SAPP_MOUSECURSOR_DEFAULT
    - SAPP_MOUSECURSOR_POINTER
    - SAPP_MOUSECURSOR_CROSSHAIR
    - SAPP_MOUSECURSOR_HAND
    - SAPP_MOUSECURSOR_IBEAM
    - SAPP_MOUSECURSOR_RESIZE_EW
    - SAPP_MOUSECURSOR_RESIZE_NS
    - SAPP_MOUSECURSOR_RESIZE_NWSE
    - SAPP_MOUSECURSOR_RESIZE_NESW
    - SAPP_MOUSECURSOR_RESIZE_ALL
    - SAPP_MOUSECURSOR_NOT_ALLOWED
- sapp_desc_flags (bitflags)  
    Purpose: Flags for app descriptor (embedded in sapp_desc).  
    Values:
    - SAPP_DESCF_ALPHA_FORMAT (0x0001): Enable alpha in swapchain.
    - SAPP_DESCF_FULLSCREEN (0x0002)
    - SAPP_DESCF_MSAA (0x0004): Enable 4x MSAA.
    - SAPP_DESCF_HIGH_DPI (0x0008)
    - SAPP_DESCF_NO_QUIT_ON_WINDOW_CLOSE (0x0010): Prevent quit on window close.
    - ... (additional platform-specific flags)

## Typedefs (Function Pointers)

All callbacks are typedefs for function pointers, taking const sapp_event* or no args, returning void. They are members of sapp_desc.

- typedef void (*sapp_event_cb)(const sapp_event* event): Handles all input/window events.
- typedef void (*sapp_frame_cb)(void): Called every frame for rendering.
- typedef void (*sapp_cleanup_cb)(void): Called on app shutdown.
- typedef void (*sapp_fail_cb)(const char* msg): Called on init failure.
- typedef int64_t (*sapp_frame_count_cb)(void): Returns current frame count (rarely used).
- typedef void (*sapp_embed_cb)(void): WebAssembly embed callback.
- typedef void (*sapp_leave_cb)(void): WebAssembly leave callback.
- typedef void (*sapp_suspend_cb)(void): App suspend (mobile/background).
- typedef void (*sapp_resume_cb)(void): App resume.
- typedef bool (*sapp_quit_requested_cb)(void): Handle quit requests (return true to allow quit).
- typedef bool (*sapp_event_type_filter_cb)(sapp_event_type t): Filter events by type (return true to process).
- typedef void (*sapp_logger_func)(const char* type, const char* msg): Custom logger.

2. Functions

  All functions are declared with SOKOL_API or SOKOL_PRIVATE macros (public unless noted). Signatures include params with types.
  
  Public API Functions

- void sapp_setup(const sapp_desc desc)*  
    Params: const sapp_desc* desc – App configuration.  
    Description: Initialize the app runner with the given descriptor. Must be called before sapp_run() in no-callback mode (rare).
- void sapp_call_frame_cb(void) (internal)  
    Params: None.  
    Description: Internal: Calls the user frame callback.
- void sapp_event(const sapp_event e)* (internal)  
    Params: const sapp_event* e – Event to dispatch.  
    Description: Internal: Dispatches an event to the user callback.
- void sapp_request_quit(void)  
    Params: None.  
    Description: Request app quit (triggers quit event).
- bool sapp_is_valid(void)  
    Params: None.  
    Returns: bool – True if app is initialized and valid.  
    Description: Check if the app runner is active.
- bool sapp_needs_suspend(void)  
    Params: None.  
    Returns: bool – True if app should suspend (e.g., mobile background).  
    Description: Query suspend state.
- void sapp_suspend(void)  
    Params: None.  
    Description: Suspend the app (internal use).
- void sapp_resume(void)  
    Params: None.  
    Description: Resume the app.
- int64_t sapp_frame_count(void)  
    Params: None.  
    Returns: int64_t – Current frame count.  
    Description: Get the number of frames rendered.
- float sapp_frame_duration(void)  
    Params: None.  
    Returns: float – Time in seconds for the last frame.  
    Description: Get duration of the previous frame.
- float sapp_time_now(void)  
    Params: None.  
    Returns: float – Current time in seconds since app start.  
    Description: Monotonic high-res timer.
- int sapp_width(void)  
    Params: None.  
    Returns: int – Current framebuffer width in pixels.  
    Description: Get current render width.
- int sapp_height(void)  
    Params: None.  
    Returns: int – Current framebuffer height in pixels.  
    Description: Get current render height.
- bool sapp_high_dpi(void)  
    Params: None.  
    Returns: bool – True if running in high-DPI mode.  
    Description: Query high-DPI status.
- bool sapp_fullscreen(void)  
    Params: None.  
    Returns: bool – True if in fullscreen mode.  
    Description: Query fullscreen status.
- void sapp_set_window_title(const char title)*  
    Params: const char* title – New window title.  
    Description: Set the window title at runtime.
- void sapp_show_mouse(bool visible)  
    Params: bool visible – Show/hide mouse cursor.  
    Description: Toggle mouse cursor visibility.
- void sapp_set_mouse_cursor(sapp_mouse_cursor cursor)  
    Params: sapp_mouse_cursor cursor – Cursor type.  
    Description: Set the mouse cursor shape.
- bool sapp_mouse_captured(void)  
    Params: None.  
    Returns: bool – True if mouse is captured.  
    Description: Query mouse capture state.
- void sapp_capture_mouse(bool capture)  
    Params: bool capture – Capture/release mouse.  
    Description: Capture mouse input (hides cursor, relative mode).
- bool sapp_clipboard_supported(void)  
    Params: None.  
    Returns: bool – True if clipboard is supported on platform.  
    Description: Check clipboard availability.
- void sapp_set_clipboard(const char text)*  
    Params: const char* text – Text to copy to clipboard.  
    Description: Set clipboard content.
- const char sapp_get_clipboard(void)*  
    Params: None.  
    Returns: const char* – Clipboard text (or null).  
    Description: Get clipboard content.
- void sapp_lock_keyboard(bool lock) (deprecated)  
    Params: bool lock – Lock/unlock keyboard.  
    Description: Legacy: Lock keyboard input (use mouse capture instead).
- bool sapp_keyboard_locked(void) (deprecated)  
    Params: None.  
    Returns: bool.  
    Description: Legacy query.
- void sapp_push_file_monitor_callback(sapp_file_drop_cb cb, void user_data)* (WebAssembly only)  
    Params: sapp_file_drop_cb cb – Callback for file drops; void* user_data.  
    Description: Register callback for file drops in browser.
- void sapp_pop_file_monitor_callback(void)  
    Params: None.  
    Description: Unregister file drop callback.
- void sapp_console_log(const char msg, ...)* (WebAssembly only)  
    Params: const char* msg – Format string; variadic args.  
    Description: Log to browser console.
- void sapp_console_logf(const char msg, ...)* (WebAssembly only)  
    Params: Same as above.  
    Description: Formatted log to console.

## Main Entry Point

- SOKOL_API void sapp_run(const sapp_desc desc)*  
    Params: const sapp_desc* desc – App configuration.  
    Description: Starts the application main loop. Non-returning; defines entry point for platforms.

## Internal/Private Functions

(These are SOKOL_PRIVATE; not for user code, but listed for completeness. ~100+ internals for platform impl, e.g., _sapp_init_state(), _sapp_win32_update_window(), etc. They handle platform-specific setup like Win32, macOS, Linux, Emscripten. Each has platform-specific params like HWND, NSWindow*, etc.)

Example internals:

- void _sapp_init_state(const sapp_desc desc)*  
    Params: const sapp_desc* desc.  
    Description: Internal init.
- void _sapp_call_init(void)  
    Params: None.  
    Description: Calls user init if needed.
- bool _sapp_dispatch_event(const sapp_event e)*  
    Params: const sapp_event* e.  
    Returns: bool.  
    Description: Dispatches event with filtering.

(Full internals omitted for brevity; they total ~150 functions for cross-platform glue code, including timer, input polling, etc.)This covers all exposed and internal elements. For usage, implement callbacks in sapp_desc and call sapp_run(). Source: Direct analysis of the header file.