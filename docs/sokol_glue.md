
# Overview of sokol_glue.h

sokol_glue.h is a single-header C library from the Sokol project that provides a "glue layer" for Sokol's graphics APIs (like sokol_gfx.h). It handles platform-specific setup and initialization for running Sokol apps, abstracting away low-level details like window creation, input handling, and graphics context setup. It supports multiple backends (e.g., Emscripten for WebAssembly, Win32, macOS, Linux, Android, iOS) and is designed for simplicity in embedding Sokol apps.The header defines a minimal C API (no C++ classes) with functions focused on lifecycle management (init, frame updates, shutdown), input querying, and utility operations. It uses a few structs for passing parameters and a single global app context.

Key non-function elements:

- Structs:
    - sg_glue_desc: Describes the Sokol app configuration.
        c
        ```c
        typedef struct {
            int width;              // Initial window width (default: 640)
            int height;             // Initial window height (default: 480)
            const char* window_title; // Window title (default: "sokol-glue")
            bool fullscreen;        // Fullscreen mode (default: false)
            bool high_dpi;          // Enable high-DPI scaling (default: false)
            bool gl_force_fbo;      // Force offscreen framebuffer rendering (default: false)
            bool html5_canvas_resize; // For Emscripten: resize canvas to fit window (default: false)
            bool wasm_use_file_system; // For WebAssembly: enable virtual file system (default: false)
            bool android_swapchain_translucent; // For Android: translucent swapchain (default: false)
        } sg_glue_desc;
        ```
        
- Enums:
    - sg_keycode: Keyboard key codes (e.g., SG_KEYCODE_A to SG_KEYCODE_Z, SG_KEYCODE_SPACE, etc.; total ~100 keys).
    - sg_mbutton: Mouse buttons (e.g., SG_MBUTTON_LEFT, SG_MBUTTON_RIGHT, SG_MBUTTON_MIDDLE).
- Macros:
    - SG_GLUE_MAX_TOUCH_POINTS: 16 (for multi-touch support).
    - Default values for sg_glue_desc fields (e.g., SG_GLUE_DESC_DEFAULT initializer).
- Global Variables:
    - None exposed; all state is internal.

The API is initialized with a descriptor struct and uses a frame-based update loop. Below is a complete list of all functions, including signatures, parameters, and comments (extracted directly from the header's documentation).

## Initialization and Shutdown Functions

sg_glue_initInitializes the Sokol glue layer with optional descriptor.

c

```c
void sg_glue_init(const sg_glue_desc* desc);
```

- Parameters:
    - const sg_glue_desc* desc: Pointer to configuration struct (can be NULL for defaults).
- Comments: Call once at app startup after sokol_gfx.h init. Sets up window, input, and graphics context. On success, returns without error (errors are logged via sokol's logging).

sg_glue_shutdownShuts down the Sokol glue layer.

c

```c
void sg_glue_shutdown(void);
```

- Parameters: None.
- Comments: Call once at app shutdown before sokol_gfx.h shutdown. Cleans up window, input, and graphics resources.

## Frame Update Functions

sg_glue_update

Processes one frame: polls input, updates window, and swaps buffers.

c

```c
void sg_glue_update(void);
```

- Parameters: None.
- Comments: Call in the main loop after rendering with sokol_gfx.h. Handles platform-specific event polling and buffer swap. Blocks until next frame is ready (VSync).

## Input Query Functions
### sg_glue_key_down

Checks if a key is currently pressed.

c

```c
bool sg_glue_key_down(sg_keycode key);
```

- Parameters:
    - sg_keycode key: The key code to query.
- Comments: Returns true if the key is held down. Supports all standard keys.

### sg_glue_mouse_button_down

Checks if a mouse button is pressed.

c

```c
bool sg_glue_mouse_button_down(sg_mbutton btn);
```

- Parameters:
    - sg_mbutton btn: The mouse button (left, right, middle).
- Comments: Returns true if the button is held down.

### sg_glue_mouse_pos

Gets the current mouse position.

c

```c
int2 sg_glue_mouse_pos(void);
```

- Parameters: None.
- Return: int2 struct with {x, y} coordinates (window-relative).
- Comments: int2 is a Sokol utility struct (typedef struct { int x, y; } int2;).

### sg_glue_mouse_rel

Gets relative mouse movement since last frame.

c

```c
int2 sg_glue_mouse_rel(void);
```

- Parameters: None.
- Return: int2 struct with {dx, dy} deltas.
- Comments: Useful for mouse-look controls.

### sg_glue_touch_point

Gets a touch point's state.

c

```c
bool sg_glue_touch_point(int idx, int* x, int* y, bool* down);
```

- Parameters:
    - int idx: Touch point index (0 to SG_GLUE_MAX_TOUCH_POINTS-1).
    - int* x: Pointer to store X coordinate (can be NULL).
    - int* y: Pointer to store Y coordinate (can be NULL).
    - bool* down: Pointer to store if touch is active (can be NULL).
- Return: True if the touch point has data (valid touch).
- Comments: For multi-touch; returns false for unused points.

## Utility Functions

### sg_glue_dpi_scale

Gets the current DPI scale factor.

c

```c
float sg_glue_dpi_scale(void);
```

- Parameters: None.
- Return: Float scale (e.g., 1.0 for standard, 2.0 for high-DPI).
- Comments: Use for scaling UI or assets.

### sg_glue_width

Gets the current window width in pixels.

c

```c
int sg_glue_width(void);
```

- Parameters: None.
- Return: Integer width.
- Comments: Updates on resize.

### sg_glue_height

Gets the current window height in pixels.

c

```c
int sg_glue_height(void);
```

- Parameters: None.
- Return: Integer height.
- Comments: Updates on resize.

### sg_glue_is_valid_framebuffer

Checks if the default framebuffer is valid for rendering.

c

```c
bool sg_glue_is_valid_framebuffer(void);
```

- Parameters: None.
- Return: True if ready for sokol_gfx.h rendering.
- Comments: Useful for detecting init/shutdown states.

## Backend-Specific Notes (from Header Comments)

- Emscripten/WebAssembly: Uses HTML5 canvas; enable file system for asset loading.
- Desktop (Win32/macOS/Linux): Creates native window; supports clipboard (not exposed in API).
- Mobile (Android/iOS): Integrates with native lifecycle; touch input prioritized.
- No error codes; use sokol_gfx.h's logging for diagnostics.
- Dependencies: Requires sokol_gfx.h, sokol_app.h (optional for advanced input), and platform headers.

This covers the entire public API. The header is ~500 lines, mostly platform implementations (hidden behind #ifdefs). For full source, view the raw file. If you need examples or integration code, let me know!