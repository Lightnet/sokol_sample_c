# Overview

sokol_time.h is a cross-platform, high-resolution timing library for measuring time intervals in applications (e.g., games, simulations). It provides tick-based timing with nanosecond-scale internal resolution.

- Tick unit: Arbitrary, platform-dependent, but internally normalized to nanoseconds.
- Precision: Sub-microsecond on most platforms.
- No dependencies beyond standard C and platform APIs.

---

## Public API Functions

|Function|Declaration|Parameters|Return Type|Description|
|---|---|---|---|---|
|stm_setup|void stm_setup(void);|None|void|Must be called once before any other function. Initializes platform-specific timers (e.g., QueryPerformanceFrequency).|
|stm_now|uint64_t stm_now(void);|None|uint64_t|Returns current timestamp in nanoseconds since stm_setup().|
|stm_diff|uint64_t stm_diff(uint64_t new_ticks, uint64_t old_ticks);|new_ticks, old_ticks|uint64_t|Returns `|
|stm_since|uint64_t stm_since(uint64_t start_ticks);|start_ticks|uint64_t|Shortcut: stm_diff(stm_now(), start_ticks) → time elapsed since start_ticks.|
|stm_laptime|uint64_t stm_laptime(uint64_t* last_time);|last_time (pointer)|uint64_t|Measures frame time or recurring interval. Returns delta since last call, updates *last_time. Returns 0 on first call.|
|stm_round_to_common_refresh_rate|uint64_t stm_round_to_common_refresh_rate(uint64_t frame_ticks);|frame_ticks|uint64_t|Snaps measured frame time to nearest common display refresh rate (60/72/75/85/90/100/120/144/240 Hz) within tolerance. Fallback: return input.|
|stm_sec|double stm_sec(uint64_t ticks);|ticks|double|Convert ticks → seconds.|
|stm_ms|double stm_ms(uint64_t ticks);|ticks|double|Convert ticks → milliseconds.|
|stm_us|double stm_us(uint64_t ticks);|ticks|double|Convert ticks → microseconds.|
|stm_ns|double stm_ns(uint64_t ticks);|ticks|double|Convert ticks → nanoseconds (identity).|

---

## Platform-Specific Backends

|Platform|Timer API Used|Notes|
|---|---|---|
|Windows|QueryPerformanceCounter / QueryPerformanceFrequency|High precision, uses LARGE_INTEGER.|
|macOS / iOS|mach_absolute_time() + mach_timebase_info|Nanosecond-scale, scaled by timebase.|
|Emscripten (Web)|emscripten_get_now()|Returns milliseconds as double, converted to ns.|
|Linux / POSIX|clock_gettime(CLOCK_MONOTONIC)|Nanosecond precision via timespec.|
|ESP8266|clock_gettime(0, ...)|Special case: ignores clock ID.|

---

## Internal State (_stm_state_t)

c

```c
static _stm_state_t _stm;
```

|Field|Type|Purpose|
|---|---|---|
|initialized|uint32_t|Magic value 0xABCDABCD to detect if stm_setup() was called.|
|Platform-specific fields:|||
|, e.g., freq, start, timebase|||

---

## Helper: _stm_int64_muldiv

c

```c
_SOKOL_PRIVATE int64_t _stm_int64_muldiv(int64_t value, int64_t numer, int64_t denom);
```

- Prevents 64-bit overflow during scaling.
- Used on Windows and macOS.
- Implements: value * numer / denom safely.

---

## Refresh Rate Snapping Table

Used in stm_round_to_common_refresh_rate():

|Refresh Rate|Frame Duration (ns)|Tolerance (ns)|
|---|---|---|
|60 Hz|16,666,667|±1,000,000|
|72 Hz|13,888,889|±250,000|
|75 Hz|13,333,333|±250,000|
|85 Hz|11,764,706|±250,000|
|90 Hz|11,111,111|±250,000|
|100 Hz|10,000,000|±500,000|
|120 Hz|8,333,333|±500,000|
|144 Hz|6,944,445|±500,000|
|240 Hz|4,166,667|±1,000,000|

Note: Table ends with {0, 0} sentinel.

---

## Macros & Configuration

|Macro|Default|Purpose|
|---|---|---|
|SOKOL_TIME_IMPL|—|Define in one file to include implementation.|
|SOKOL_ASSERT(c)|assert(c)|Custom assert.|
|SOKOL_TIME_API_DECL|extern|Function visibility (DLL export/import on Windows).|
|SOKOL_DLL|—|Enable DLL export/import on Windows.|

---

## Usage Example

c

```c
#define SOKOL_IMPL
#include "sokol_time.h"

int main() {
    stm_setup();

    uint64_t t0 = stm_now();
    // ... do work ...
    uint64_t elapsed_ns = stm_since(t0);

    printf("Elapsed: %.3f ms\n", stm_ms(elapsed_ns));

    // Frame timing
    uint64_t last = 0;
    while (running) {
        uint64_t frame_time = stm_laptime(&last);
        double fps = 1.0 / stm_sec(frame_time);
        // ...
    }
}
```

---

## Key Notes

- All time is in nanoseconds internally.
- stm_diff returns at least 1 → avoids division by zero.
- stm_laptime is ideal for frame delta timing.
- stm_round_to_common_refresh_rate helps stabilize FPS reporting.
- Thread-safe after stm_setup() (uses monotonic clocks).
- No dynamic allocation.

---

## Summary Table (Quick Reference)

|Function|Params|Returns|Use Case|
|---|---|---|---|
|stm_setup()|—|—|Init once|
|stm_now()|—|uint64_t|Current time|
|stm_diff(a,b)|2×uint64_t|uint64_t|`|
|stm_since(t)|uint64_t|uint64_t|Time since t|
|stm_laptime(&t)|uint64_t*|uint64_t|Frame time|
|stm_round_to_common_refresh_rate(t)|uint64_t|uint64_t|Snap to 60/120/etc Hz|
|stm_sec/ms/us/ns(t)|uint64_t|double|Convert to units|

---

Project: [https://github.com/floooh/sokol](https://github.com/floooh/sokol)  
License: zlib/libpng  
Author: Andre Weissflog (@floooh)