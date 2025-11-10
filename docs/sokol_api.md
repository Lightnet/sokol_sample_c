
# sokol log:
Log Level Values (Hardcoded)

|Level|Meaning|Value|
|---|---|---|
|0|panic|aborts|
|1|error||
|2|warning||
|3|info||

These are not enums — just uint32_t values.

# custom log:
```c

/* -------------------------------------------------------------
   Logging helper – safe name, variadic, passes line/file
   ------------------------------------------------------------- */
static void app_logf(const char* tag,
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
    app_logf(tag, 3, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#define LOG_WARN(tag, fmt, ...) \
    app_logf(tag, 2, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

#define LOG_ERROR(tag, fmt, ...) \
    app_logf(tag, 1, fmt, __LINE__, __FILE__, ##__VA_ARGS__)

```

```c
    LOG_INFO("mygame", "init...");
```

```c
    static int fps = 0;
    fps = (int)(1.0f / sapp_frame_duration());
    LOG_INFO("mygame", "Game running at FPS: %.1f", fps);
```

```c

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    printf("main...\n");

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

# sokol app:


# sokol gfx: