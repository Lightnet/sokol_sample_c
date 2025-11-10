Here is a complete summary of functions, parameters, and key information from the sokol_log.h header and its implementation.

---

Header: sokol_log.hMain Function (Public API)

c

```c
SOKOL_LOG_API_DECL void slog_func(
    const char* tag,
    uint32_t log_level,
    uint32_t log_item,
    const char* message,
    uint32_t line_nr,
    const char* filename,
    void* user_data
);
```

Parameters

|Parameter|Type|Description|
|---|---|---|
|tag|const char*|Short identifier for the subsystem (e.g., "sspine", "saudio"). Can be NULL.|
|log_level|uint32_t|Log severity level: 0 = panic 1 = error 2 = warning ≥3 = info|
|log_item|uint32_t|Numeric ID for the specific log item (used in validation macros in other sokol headers).|
|message|const char*|Human-readable log message. Can be NULL.|
|line_nr|uint32_t|Source code line number where log was triggered.|
|filename|const char*|Source file path. Can be NULL.|
|user_data|void*|Optional user-provided pointer (passed through from setup). Currently unused.|

---

Implementation: sokol_log_impl.c (when SOKOL_LOG_IMPL is defined)Internal Helper Functions

|Function|Purpose|
|---|---|
|_slog_append(const char* str, char* dst, char* end)|Safely appends a string to a buffer with bounds checking. Returns new position.|
|_slog_itoa(uint32_t x, char* buf, size_t buf_size)|Converts uint32_t to decimal string (no snprintf). Returns pointer to start of digits.|

---

Platform Detection Macros

|Macro|Platform|
|---|---|
|_SLOG_APPLE|macOS / iOS|
|_SLOG_EMSCRIPTEN|Web (Emscripten)|
|_SLOG_WINDOWS|Windows|
|_SLOG_ANDROID|Android|
|_SLOG_LINUX|Linux / Unix|

---

Output Destinations by Platform

|Platform|stderr|Additional Output|
|---|---|---|
|Windows|Yes|OutputDebugStringA()|
|macOS/iOS|Yes|syslog()|
|Linux|Yes|syslog()|
|Android|No|__android_log_write()|
|Emscripten|No|console.info/warn/error via JS|

Note: On Apple/Linux, both stderr and syslog are used.

---

Log Message FormatDebug Mode (SOKOL_DEBUG defined, i.e., not NDEBUG)

```text
[tag][level][id:X] filename:line:0:
    message
```

Example:

```text
[sspine][error][id:12] /Users/floh/projects/sokol/util/sokol_spine.h:3472:0:
    SKELETON_DESC_NO_ATLAS: no atlas object provided in sspine_skeleton_desc.atlas
```

Release Mode (no SOKOL_DEBUG)

```text
[tag][level][id:X][line:Y]
```

Example:

```text
[sspine][error][id:12][line:3472]
```

This reduces binary size by omitting full file paths and messages.

---

Special Behavior: log_level == 0 (Panic)

- Appends: "ABORTING because of [panic]\n"
- Calls abort() unconditionally
- Must not return (per documentation rules)

---

Usage Example (from docs)

c

```c
#include "sokol_log.h"
#include "sokol_audio.h"

saudio_setup(&(saudio_desc){
    .logger.func = slog_func
});
```

Same pattern applies to all sokol libraries: sg_setup, sapp_desc, sspine_desc, etc.

---

Configuration Macros

|Macro|Default|Effect|
|---|---|---|
|SOKOL_LOG_IMPL|—|Enables implementation|
|SOKOL_DEBUG|Defined if !NDEBUG|Enables verbose logging|
|SOKOL_ASSERT(c)|assert(c)|Custom assert|
|SOKOL_UNREACHABLE()|assert(false)|Mark unreachable code|
|SOKOL_LOG_API_DECL|extern or DLL import/export|Controls linkage|

---

Line Buffer Size

c

```c
#define _SLOG_LINE_LENGTH (512)
```

All log output is built into a 512-byte stack buffer. Long messages are truncated.

---

Thread Safety

- Re-entrant: Yes
- Thread-safe: Yes (uses only local buffers and platform APIs assumed safe)

---

Summary Table: slog_func Log Levels

|Level|String|Color/Console|Action|
|---|---|---|---|
|0|panic|console.error / FATAL|abort()|
|1|error|console.error / ERROR|—|
|2|warning|console.warn / WARN|—|
|≥3|info|console.info / INFO|—|

---

Key Design Notes

- No dynamic allocation
- Zero-terminated strings assumed
- All string params optional (NULL safe)
- Clickable error format for IDEs (VS Code, CLion, etc.)
- Cross-platform with fallback to stderr

---

Final Answer: All Functions & Info

|Item|Details|
|---|---|
|Public Function|slog_func(...)|
|Parameters|tag, log_level, log_item, message, line_nr, filename, user_data|
|Log Levels|0=panic, 1=error, 2=warning, ≥3=info|
|Internal Helpers|_slog_append, _slog_itoa|
|Buffer Size|512 bytes|
|Panic Behavior|Appends abort message + abort()|
|Platforms|Windows, macOS, iOS, Linux, Android, Web (Emscripten)|
|Output|stderr + platform log (syslog, OutputDebugString, Android log, JS console)|
|Debug vs Release|Full path+msg vs minimal tags|
|Thread Safe|Yes|
|Re-entrant|Yes|

---

Let me know if you want a minimal custom logger or how to suppress logging in release.