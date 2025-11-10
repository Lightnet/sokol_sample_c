
# Overview of sokol_gfx.h

sokol_gfx.h is a single-header C library for cross-platform graphics programming, providing a simple API for initializing graphics backends (WebGPU, D3D11, Metal, OpenGL, GLES2/3), managing resources (buffers, images, shaders, pipelines, and passes), and issuing draw commands. It is part of the Sokol ecosystem and is designed for low-level control with minimal boilerplate. The header includes enums, structs, and macros for configuration, validation, and utilities. Below is an exhaustive list of all public functions (excluding internal/private ones), grouped by logical category based on the header's structure and comments. Each entry includes:

- Signature: Full function declaration with return type, name, parameters (types and names), and any default values.
- Description: Purpose and key notes from inline comments or docstrings.
- Parameters: Detailed breakdown if additional context is provided in comments.

Functions are declared as inline static where applicable for performance. The header uses #define SOKOL_IMPL to include implementations (not listed here, as they are in the .c-like expansion).

1. Initialization and ShutdownThese functions handle backend setup, context management, and cleanup.

|Function|Signature|Description|Parameters|
|---|---|---|---|
|sg_setup|void sg_setup(const sg_desc* desc)|Initializes the Sokol graphics context with the provided descriptor. Must be called before any other functions. Supports multiple backends via desc->context.|- desc: Pointer to sg_desc struct containing global settings (e.g., buffer pool sizes, MTL device, GL context callbacks, logging, validation layers).|
|sg_shutdown|void sg_shutdown(void)|Shallows the Sokol graphics context, releasing all resources and resetting internal state. Call at application exit.|None.|
|sg_isvalid|bool sg_isvalid(void)|Returns true if the graphics context is valid (after sg_setup and before sg_shutdown).|None.|
|sg_reset_state_cache|void sg_reset_state_cache(void)|Resets the internal state cache (e.g., for resource rebinding). Useful in dynamic scenarios but expensive.|None.|

2. Resource Creation and DestructionFunctions for allocating and freeing GPU resources like buffers, images, shaders, pipelines, and passes.

|Function|Signature|Description|Parameters|
|---|---|---|---|
|sg_make_buffer|sg_buffer sg_make_buffer(const sg_buffer_desc* desc)|Creates a buffer resource (vertex/index buffer or uniform buffer). Returns an invalid ID on failure.|- desc: Pointer to sg_buffer_desc with size, content pointer/data, type (vertex/index/uniform), usage (immutable/dynamic/stream), etc.|
|sg_destroy_buffer|void sg_destroy_buffer(sg_buffer buf)|Destroys a buffer resource, releasing GPU memory. Invalid IDs are ignored.|- buf: The buffer handle to destroy.|
|sg_make_image|sg_image sg_make_image(const sg_image_desc* desc)|Creates an image resource (texture or render target). Supports 1D/2D/3D/cube, mipmaps, layers, formats, etc.|- desc: Pointer to sg_image_desc with type, dimensions, format, render targets, sampler state, initial data per slice/mip.|
|sg_destroy_image|void sg_destroy_image(sg_image img)|Destroys an image resource.|- img: The image handle to destroy.|
|sg_make_shader|sg_shader sg_make_shader(const sg_shader_desc* desc)|Creates a shader program from bytecode (SPIR-V/GLSL). Binds vertex/layout stages.|- desc: Pointer to sg_shader_desc with attrs (vertex inputs), vs/fs byte code sizes/pointers, entry points.|
|sg_destroy_shader|void sg_destroy_shader(sg_shader shd)|Destroys a shader resource.|- shd: The shader handle to destroy.|
|sg_make_pipeline|sg_pipeline sg_make_pipeline(const sg_pipeline_desc* desc)|Creates a render pipeline state object (blending, depth/stencil, primitive type, etc.).|- desc: Pointer to sg_pipeline_desc with shader, layout (vertex buffers/attrs), primitive type, rasterizer/depth/stencil/color states.|
|sg_destroy_pipeline|void sg_destroy_pipeline(sg_pipeline pip)|Destroys a pipeline resource.|- pip: The pipeline handle to destroy.|
|sg_make_pass|sg_pass sg_make_pass(const sg_pass_desc* desc)|Creates an offscreen render pass with attached images for color/depth.|- desc: Pointer to sg_pass_desc with color attachments (image slots, slice/mip/layer), depth attachment.|
|sg_destroy_pass|void sg_destroy_pass(sg_pass pass)|Destroys a pass resource.|- pass: The pass handle to destroy.|

3. Buffer and Image UpdatesFunctions to update resource contents dynamically.

|Function|Signature|Description|Parameters|
|---|---|---|---|
|sg_update_buffer|void sg_update_buffer(sg_buffer buf, const void* data, int size)|Updates the content of a dynamic or stream buffer. Can only be called outside render passes.|- buf: Target buffer. - data: Pointer to new data. - size: Size in bytes (must match desc size).|
|sg_update_image|void sg_update_image(sg_image img, const sg_image_data* data)|Updates image content (e.g., texture uploads). Supports per-mip/slice data.|- img: Target image. - data: Pointer to sg_image_data with content arrays for each mip/slice (size must match).|
|sg_update_buffer_range|void sg_update_buffer_range(sg_buffer buf, int offset, const void* data, int size)|Partial update of a buffer range (for dynamic buffers).|- buf: Target buffer. - offset: Byte offset. - data: Pointer to data. - size: Size in bytes.|
|sg_update_image_range|void sg_update_image_range(sg_image img, int mip_level, int slice, const sg_image_subimage_content* data) (Note: Actual sig is more general; see header for overloads)|Partial update of image subregions (mipmap/slice).|- img: Target image. - mip_level: Mipmap level. - slice: Array layer (for 2D arrays/cubes). - data: Pointer to content struct with subimage ptr/size.|

4. Rendering: Passes and CommandsFunctions for beginning/ending render passes and issuing draw calls.

|Function|Signature|Description|Parameters|
|---|---|---|---|
|sg_begin_pass|void sg_begin_pass(sg_pass pass, const sg_pass_action* action)|Begins a render pass (default or offscreen). Clears attachments per action.|- pass: Pass handle (default: SG_DEFAULT_PASS). - action: Pointer to sg_pass_action for clear colors, depth, stencil values; load/clear ops.|
|sg_end_pass|void sg_end_pass(void)|Ends the current render pass, submitting commands to GPU.|None.|
|sg_apply_bindings|void sg_apply_bindings(const sg_bindings* bindings)|Binds vertex buffers, index buffer, vertex buffers, images, and FBs for the current pass.|- bindings: Pointer to sg_bindings with vs_fs_images, vertex_buffer_ids, instance_buffer_ids, index_buffer_id.|
|sg_apply_pipeline|void sg_apply_pipeline(sg_pipeline pip)|Binds a pipeline state object for subsequent draws.|- pip: Pipeline handle.|
|sg_apply_uniforms|void sg_apply_uniforms(sg_shader_stage stage, int ub_index, const sg_range* data)|Updates uniform data for a shader stage (VS or FS).|- stage: SG_VERTEXSHADER or SG_FRAGMENTSHADER. - ub_index: Uniform block index. - data: Pointer to sg_range (ptr/size for uniform data).|
|sg_draw|void sg_draw(int base_element, int num_elements, int num_instances)|Issues a draw call (indexed or non-indexed based on pipeline).|- base_element: Starting vertex/index. - num_elements: Number of elements to draw. - num_instances: Instanced count (1 for non-instanced).|
|sg_draw_base_vertex|void sg_draw_base_vertex(int base_vertex, int base_instance, int num_vertices, int num_instances)|Draw call with base vertex/instance offsets (for instancing).|- base_vertex: Vertex offset. - base_instance: Instance offset. - num_vertices: Vertex count. - num_instances: Instance count.|
|sg_draw_indexed_base_vertex|void sg_draw_indexed(int base_element, int num_elements, int base_vertex) (Note: Extended variants exist)|Indexed draw with base vertex offset.|- base_element: Index offset. - num_elements: Element count. - base_vertex: Vertex base.|

5. Query and Debugging FunctionsUtility functions for resource info, errors, and validation.

|Function|Signature|Description|Parameters|
|---|---|---|---|
|sg_query_buffer_info|sg_buffer_info sg_query_buffer_info(sg_buffer buf)|Returns runtime info about a buffer (e.g., slot, state).|- buf: Buffer handle. Returns sg_buffer_info struct.|
|sg_query_image_info|sg_image_info sg_query_image_info(sg_image img)|Returns runtime info about an image (slots, updates, state).|- img: Image handle. Returns sg_image_info struct.|
|sg_query_shader_info|sg_shader_info sg_query_shader_info(sg_shader shd)|Returns runtime info about a shader.|- shd: Shader handle. Returns sg_shader_info struct.|
|sg_query_pipeline_info|sg_pipeline_info sg_query_pipeline_info(sg_pipeline pip)|Returns runtime info about a pipeline.|- pip: Pipeline handle. Returns sg_pipeline_info struct.|
|sg_query_pass_info|sg_pass_info sg_query_pass_info(sg_pass pass)|Returns runtime info about a pass.|- pass: Pass handle. Returns sg_pass_info struct.|
|sg_describe_buffer|const char* sg_describe_buffer(sg_buffer buf)|Human-readable description for debugging (e.g., "buffer #3").|- buf: Buffer handle.|
|(Similar for other resources)|const char* sg_describe_image(sg_image img) etc.|Analogous debug strings for image, shader, pipeline, pass.|- Resource handle.|
|sg_trace|(Macro, not function)|N/A (logging macro; see header for usage).|N/A.|
|sg_push_debug_text|void sg_push_debug_text(const char* text)|Pushes debug text to the debug layer (if enabled).|- text: Null-terminated string.|

Additional Notes

- Structs and Enums: The header defines numerous supporting types (e.g., sg_desc, sg_buffer_desc, sg_color, sg_range, enums like sg_resource_state, sg_pixel_format, sg_usage, sg_primitive_type, sg_cubeface, sg_shader_stage, sg_pass_action). These are used as parameters but not functions themselves.
- Macros: Includes validation macros (e.g., SOKOL_VALIDATE_BEGIN()), resource ID generation (SG_INVALID_ID), and feature queries (e.g., SG_FEATURE_ORIGIN_BOTTOM_LEFT).
- Backend-Specific: Functions like sg_mtl_device() return pointers to native objects (e.g., id<MTLDevice> for Metal).
- Error Handling: Most creation functions return invalid IDs on failure; check with sg_isvalid_resource(). Enable validation via desc->validation_layers.
- Threading: Not thread-safe; all calls must be from the main thread unless specified.
- Total Functions: ~50+ including variants; the above covers core public API. For exact byte-for-byte details, refer to the raw header.

This list is derived from the header's content as of the current date. If you need details on specific structs/enums or examples, let me know!