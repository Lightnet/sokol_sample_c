
Below is a complete reference of every public function defined in sokol_fetch.h (and its implementation), together with all parameters, return types, and a short description of what the function does.  
The list follows the order in which the functions appear in the header file.

---

1. Setup / Shutdown

|Function|Return|Parameters|Description|
|---|---|---|---|
|sfetch_setup(const sfetch_desc_t* desc)|void|const sfetch_desc_t* desc|Initialises a thread-local sokol_fetch instance. Must be called once per thread before any other API call. Allocates the request pool and starts IO threads.|
|sfetch_shutdown(void)|void|–|Destroys the thread-local instance, stops all IO threads, frees all memory. Call at the end of the program (or thread).|
|sfetch_valid(void)|bool|–|Returns true if sfetch_setup() succeeded and the context is still alive.|
|sfetch_desc(void)|sfetch_desc_t|–|Returns a copy of the sfetch_desc_t that was passed to sfetch_setup() (with defaults filled in).|
|sfetch_max_userdata_bytes(void)|int|–|Returns the maximum size of the user-data block in bytes (SFETCH_MAX_USERDATA_UINT64 * 8).|
|sfetch_max_path(void)|int|–|Returns the maximum length of a path/URL string (SFETCH_MAX_PATH).|

---

2. Sending a Request

|Function|Return|Parameters|Description|
|---|---|---|---|
|sfetch_send(const sfetch_request_t* request)|sfetch_handle_t|const sfetch_request_t* request|Enqueues a new fetch request. Returns a handle that identifies the request (or an invalid handle {0} if the pool is exhausted).|

sfetch_request_t fields (all optional except path and callback)

|Field|Type|Default|Meaning|
|---|---|---|---|
|channel|uint32_t|0|Index of the IO channel (0 … num_channels-1).|
|path|const char*|required|UTF-8 filesystem path or HTTP URL. Max length = SFETCH_MAX_PATH-1.|
|callback|void (*)(const sfetch_response_t*)|required|Function called when the request needs attention.|
|chunk_size|uint32_t|0|If >0 → streaming; data is delivered in chunks of this size (uncompressed).|
|buffer|sfetch_range_t|{0,0}|Memory where data will be written. Must be large enough for the whole file or for one chunk.|
|user_data|sfetch_range_t|{0,0}|POD blob copied into the request (max SFETCH_MAX_USERDATA_UINT64*8 bytes).|

---

3. Request Lifetime & Control

|Function|Return|Parameters|Description|
|---|---|---|---|
|sfetch_handle_valid(sfetch_handle_t h)|bool|sfetch_handle_t h|Returns true only if the handle is valid and the request is still alive.|
|sfetch_pause(sfetch_handle_t h)|void|sfetch_handle_t h|Pauses a streaming request. The response callback will be invoked each frame with paused == true.|
|sfetch_continue(sfetch_handle_t h)|void|sfetch_handle_t h|Resumes a paused request.|
|sfetch_cancel(sfetch_handle_t h)|void|sfetch_handle_t h|Cancels a request immediately. The callback is called once with cancelled + finished.|
|sfetch_bind_buffer(sfetch_handle_t h, sfetch_range_t buffer)|void|sfetch_handle_t h, sfetch_range_t buffer|Attaches a buffer to a request from inside the response callback. Must be called only when no buffer is bound yet.|
|sfetch_unbind_buffer(sfetch_handle_t h)|void*|sfetch_handle_t h|Detaches the current buffer and returns its pointer (useful for freeing dynamic memory). Must be called from the callback.|

---

4. Per-Frame Work

|Function|Return|Parameters|Description|
|---|---|---|---|
|sfetch_dowork(void)|void|–|Pump the internal message queues, move requests in/out of IO threads, invoke response callbacks. Call once per frame (or more often if you need lower latency).|

---

5. Helper Types

|Type|Members|Notes|
|---|---|---|
|sfetch_range_t|const void* ptr; size_t size;|Pointer + byte count. Use macro SFETCH_RANGE(x) to initialise from a value.|
|sfetch_handle_t|uint32_t id;|Opaque handle, thread-local and generation-counter protected.|
|sfetch_error_t|enum (SFETCH_ERROR_NO_ERROR, FILE_NOT_FOUND, NO_BUFFER, BUFFER_TOO_SMALL, UNEXPECTED_EOF, INVALID_HTTP_STATUS, CANCELLED, JS_OTHER)|Detailed error code when failed == true.|
|sfetch_response_t|(see table below)|Passed to the user callback.|

sfetch_response_t fields (read-only for the user)

|Field|Type|Meaning|
|---|---|---|
|handle|sfetch_handle_t|The request this response belongs to.|
|dispatched|bool|true once when a lane has been assigned (buffer can be bound here).|
|fetched|bool|true when a chunk of data is ready in data.|
|paused|bool|true while the request is paused.|
|finished|bool|Last callback for this request (success, failure or cancelled).|
|failed|bool|true if something went wrong (always with finished).|
|cancelled|bool|true if the request was cancelled by sfetch_cancel().|
|error_code|sfetch_error_t|Detailed error when failed.|
|channel|uint32_t|Channel index.|
|lane|uint32_t|Lane index on that channel (useful for pre-allocated buffer pools).|
|path|const char*|Original path/URL.|
|user_data|void*|Pointer to the copied user-data block (8-byte aligned).|
|data_offset|uint32_t|Byte offset of the current chunk inside the whole file (streaming only).|
|data|sfetch_range_t|Fetched data (ptr == buffer.ptr, size ≤ buffer.size).|
|buffer|sfetch_range_t|The buffer that was bound to the request.|

---

6. Configuration (sfetch_desc_t)

|Field|Type|Default|Meaning|
|---|---|---|---|
|max_requests|uint32_t|128|Total number of requests that can be alive across all channels.|
|num_channels|uint32_t|1|Number of parallel IO channels (each gets its own thread on native platforms).|
|num_lanes|uint32_t|1|Max requests in-flight per channel (rate-limiting).|
|allocator|sfetch_allocator_t|{malloc, free}|Custom memory allocation callbacks.|
|logger|sfetch_logger_t|none|Optional logging callback (highly recommended: slog_func).|

---

7. Logging / Errors (via sfetch_logger_t)

c

```c
typedef void (*sfetch_log_func_t)(
    const char* tag,               // always "sfetch"
    uint32_t log_level,            // 0=panic, 1=error, 2=warning, 3=info
    uint32_t log_item_id,          // SFETCH_LOGITEM_*
    const char* message_or_null,
    uint32_t line_nr,
    const char* filename_or_null,
    void* user_data);
```

Log items (SFETCH_LOGITEM_*) are defined via X-macros; the implementation prints a human-readable string in debug builds.

---

8. Platform-Specific Details (implementation only)

|Platform|Threading|File API|HTTP|
|---|---|---|---|
|POSIX (Linux, macOS, etc.)|pthread|fopen/fread/fseek/fclose|none (local files only)|
|Windows|Win32 threads|CreateFileW/ReadFile/SetFilePointerEx|none|
|Emscripten / WASM|no threads|–|fetch() API (HEAD + GET, range requests)|

The public API is identical on all platforms.

---

TL;DR – Minimal usage skeleton

c

```c
sfetch_setup(&(sfetch_desc_t){ .logger.func = slog_func });

static uint8_t file_buf[1<<20];
sfetch_handle_t h = sfetch_send(&(sfetch_request_t){
    .path = "data.bin",
    .callback = my_cb,
    .buffer = SFETCH_RANGE(file_buf)
});

void my_cb(const sfetch_response_t* r) {
    if (r->fetched)   { /* use r->data.ptr / r->data.size */ }
    if (r->finished)  { /* cleanup */ }
}

while (running) {
    sfetch_dowork();
    /* ... render ... */
}
sfetch_shutdown();
```

That’s everything you can call from user code.