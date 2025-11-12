# sokol_sample_c

# License: MIT

# Features:
- [x] cimgui test.
- [x] triangle
- [x] quad 
- [x] quad wireframe
- [x] quad index
- [ ] 2d plane texture
- [x] cube
- [x] texture cube
- [x] load file test
- [x] custom log
- [ ] 

# sokol tag:
- ce5f7c96010a76876d4c15f3e618ffbe583991e1   Nov 6, 2025

# Information:
  It is base on sokol starter cimgui repo. This is just a sample for sokol build tests. To work toward c program language build project.

## Notes:
- It not like raylib cheatsheet. More low level access but compatible for OS.
- Limited docs required look at the header files.
- There are good examples of the github from floooh and other devs.
- Need to match the header files for those functions from out date code to keep it simple.
- cimgui stil use c++ to wrapper for ease quick gui build.
- some headers are out date for stb image.

# KISS (Keep It Simple, Stupid):
  Note due to dev from sokol is keep it simple and let the dev create their own features to extend.

## logging:
  Sokol app is disable by default. It has to be config to enable logging.

```c
/* -------------------------------------------------------------
   Logging helper – safe name, variadic, passes line/file
   ------------------------------------------------------------- */
static void slogf(const char* tag,
                     uint32_t level,
                     const char* fmt,
                     uint32_t line,
                     const char* file,
                     ...)               /* variadic starts here */
{
    char buf[1024];
    va_list ap;
    va_start(ap, file);                    // start after 'file'
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    slog_func(tag, level, 0, buf, line, file, NULL);
}

/* -----------------------------------------------------------------
   Helper macros – note the __LINE__/__FILE__ are now part of the macro
   ----------------------------------------------------------------- */
#define LOG_INFO(tag, fmt, ...) \
    slogf(tag, 3, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#define LOG_WARN(tag, fmt, ...) \
    slogf(tag, 2, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#define LOG_ERROR(tag, fmt, ...) \
    slogf(tag, 1, fmt, __LINE__, __FILE__, ##__VA_ARGS__)
```

```c
LOG_INFO("mygame", "init...");
```

```c
//...
sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    printf("main...\n");// this will not show up depend on the os which is windows os.

    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = "Sokol Window",
        .width = 800,
        .height = 600,
        .icon.sokol_default = true,

        .logger.func = slog_func,
        .win32_console_utf8 = true,
        //.win32_console_create = true, // this create console terminal. this will not work when ide.
        .win32_console_attach = true, // this for ide for terminal.
    };
}
```

# User data:
  It handle custom data like context. Need to read doc.

# Guide:
  Work in progress.
  
  Sokol only use header to keep things minimal. To make it compatible for export to cross platforms. Read more about the to link: https://github.com/floooh/sokol . There is no version for sokol github from I get from it. I can only use data commit date version as reference Nov 6, 2025. Note that there will be changes for files for sokol github to make the api to be simple as possible.

  You need to get sokol shader tool to run this sample project. Sokol have build or download the tool for shader c programing language. It can be found in sokol github in the read me file for links. It required python, ninja and other tools to build the shader tool.

## triangle.glsl
```
@vs vs
in vec4 position;

void main() {
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}
@end

@fs fs
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
@end

@program simple vs fs
```
## command line:
```
./sokol-shdc.exe --input shaders/triangle.glsl --output include/triangle.glsl.h --format sokol --slang glsl410
```
  Note args might change for how compile to header file. Check using the help command line or read the docs for shaderc tool github.

# Credits:
- https://github.com/floooh/sokol
- https://github.com/zeromake/learnopengl-examples (out date, refs)
- https://github.com/floooh/sokol-samples
- https://github.com/floooh/sokol-tools 
- https://github.com/floooh/sokol-tools-bin
- 
