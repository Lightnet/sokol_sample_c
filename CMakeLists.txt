cmake_minimum_required(VERSION 3.20)
project(sokol-sample)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CIMGUI_PATH_DIR libs/cimgui)

#=== LIBRARY: cimgui + Dear ImGui
add_library(cimgui STATIC
    ${CIMGUI_PATH_DIR}/cimgui.cpp
    ${CIMGUI_PATH_DIR}/cimgui.h
    ${CIMGUI_PATH_DIR}/imgui.cpp
    ${CIMGUI_PATH_DIR}/imgui.h
    ${CIMGUI_PATH_DIR}/imgui_widgets.cpp
    ${CIMGUI_PATH_DIR}/imgui_draw.cpp
    ${CIMGUI_PATH_DIR}/imgui_tables.cpp
    ${CIMGUI_PATH_DIR}/imgui_demo.cpp
)
target_include_directories(cimgui INTERFACE ${CIMGUI_PATH_DIR})

#=== LIBRARY: sokol
set(SOKOL_PATH_DIR libs/sokol)
# add headers to the the file list because they are useful to have in IDEs
set(SOKOL_HEADERS
    ${SOKOL_PATH_DIR}/sokol_gfx.h
    ${SOKOL_PATH_DIR}/sokol_app.h
    ${SOKOL_PATH_DIR}/sokol_imgui.h
    ${SOKOL_PATH_DIR}/sokol_glue.h
    ${SOKOL_PATH_DIR}/sokol_fetch.h
)
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
    target_compile_options(sokol PRIVATE -x objective-c)
    target_link_libraries(sokol PUBLIC
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework MetalKit"
        "-framework Metal")
else()
    add_library(sokol STATIC ${SOKOL_PATH_DIR}/sokol.c ${SOKOL_HEADERS})
    if (CMAKE_SYSTEM_NAME STREQUAL Linux)
        target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL dl m)
        target_link_libraries(sokol PUBLIC Threads::Threads)
    endif()
endif()
target_link_libraries(sokol PUBLIC cimgui)
target_include_directories(sokol INTERFACE ${SOKOL_PATH_DIR})

set(STB_PATH_DIR libs/stb)
set(LIBS_INCLUDE_DIR libs)

set(SRC_FILES
    ${LIBS_INCLUDE_DIR}/nuklear/nuklear.c #need for nuklear setup.
    ${LIBS_INCLUDE_DIR}/util/fileutil.c
    ${LIBS_INCLUDE_DIR}/stb/stb_image.c
)

set(APP_NAME demo)
set(MAIN_FILE
    ${SRC_FILES}
    # src/demo.c
    # src/sokol_flecs.c
    # examples/sokol_window.c
    # examples/triangle.c
    # examples/quad.c
    # examples/quad_wireframe.c
    # examples/quad_index.c
    # examples/sokol_input.c
    # examples/sokol_quad_texture.c
    # examples/sokol_nuklear.c
    # examples/sbuftex_sapp.c # nope
    examples/loadpng_sapp.c
)
#=== EXECUTABLE: demo
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(${APP_NAME} WIN32 ${MAIN_FILE})
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT demo)
else()
    add_executable(${APP_NAME} ${MAIN_FILE})
endif()
target_link_libraries(${APP_NAME} 
    sokol
    # flecs                                           # flecs
)
target_include_directories(${APP_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${STB_PATH_DIR}
    ${LIBS_INCLUDE_DIR}
)
# this hack removes the xxx-CMakeForceLinker.cxx dummy file
set_target_properties(${APP_NAME} PROPERTIES LINKER_LANGUAGE C)







# include(FetchContent)

# FetchContent_Declare(
#     sokol
#     GIT_REPOSITORY https://github.com/floooh/sokol.git
#     GIT_TAG master
#     GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(sokol)
# FetchContent_Declare(
#     flecs
#     GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
#     GIT_TAG v4.1.2
#     GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(flecs)
# FetchContent_Declare(
#     imgui
#     GIT_REPOSITORY https://github.com/ocornut/imgui.git
#     GIT_TAG v1.92.4
#     GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(imgui)
# FetchContent_Declare(
#     cimgui
#     GIT_REPOSITORY https://github.com/cimgui/cimgui.git
#     GIT_TAG docking_inter
#     GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(cimgui)

# add_library(libcimgui STATIC
#     ${cimgui_SOURCE_DIR}/cimgui.cpp
#     ${cimgui_SOURCE_DIR}/imgui/imgui.cpp
#     ${cimgui_SOURCE_DIR}/imgui/imgui_widgets.cpp
#     ${cimgui_SOURCE_DIR}/imgui/imgui_draw.cpp
#     ${cimgui_SOURCE_DIR}/imgui/imgui_tables.cpp
#     ${cimgui_SOURCE_DIR}/imgui/imgui_demo.cpp
# )
# target_include_directories(libcimgui PUBLIC
#     ${cimgui_SOURCE_DIR}
#     ${cimgui_SOURCE_DIR}/imgui
# )
# # Set compile definitions for custom_cimgui (no CIMGUI_DEFINE_ENUMS_AND_STRUCTS)
# target_compile_definitions(libcimgui PUBLIC
#     IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
#     IMGUI_IMPL_API=extern\ \"C\"
#     IMGUI_DEFINE_ENUMS_AND_STRUCTS=1  
# )


#=== LIBRARY: sokol
# add headers to the the file list because they are useful to have in IDEs
# message(STATUS "sokol_SOURCE_DIR >> ${sokol_SOURCE_DIR}")
# message(STATUS "sokol_app.h >> ${sokol_SOURCE_DIR}/sokol_app.h")
# set(SOKOL_HEADERS
#     ${sokol_SOURCE_DIR}/sokol_gfx.h
#     ${sokol_SOURCE_DIR}/sokol_app.h
#     ${sokol_SOURCE_DIR}/util/sokol_imgui.h
#     ${sokol_SOURCE_DIR}/sokol_glue.h
# )
# add_library(sokol STATIC 
#     # sokol/sokol.c 
#     ${SOKOL_HEADERS}
#     # ${CMAKE_CURRENT_SOURCE_DIR}/sokol/sokol.c
#     sokol/sokol.c
# )
# target_include_directories(sokol PUBLIC
#     # ${CMAKE_CURRENT_SOURCE_DIR}/sokol
#     ${sokol_SOURCE_DIR}
#     ${sokol_SOURCE_DIR}/util
#     cimgui
# )
# target_link_libraries(sokol PUBLIC 
#     cimgui
#     # libcimgui
# )
# # target_include_directories(csokol INTERFACE sokol)
# target_include_directories(sokol INTERFACE 
#     ${sokol_SOURCE_DIR}
#     cimgui
# )
# # Define implementation for sokol.c
# # target_compile_definitions(sokol PRIVATE SOKOL_IMPL)

# set(CIMGUI_PATH libs/cimgui)

# #=== LIBRARY: cimgui + Dear ImGui
# add_library(cimgui STATIC
#     ${CIMGUI_PATH}/cimgui.cpp
#     ${CIMGUI_PATH}/cimgui.h
#     ${CIMGUI_PATH}/imgui.cpp
#     ${CIMGUI_PATH}/imgui.h
#     ${CIMGUI_PATH}/imgui_widgets.cpp
#     ${CIMGUI_PATH}/imgui_draw.cpp
#     ${CIMGUI_PATH}/imgui_tables.cpp
#     ${CIMGUI_PATH}/imgui_demo.cpp
# )
# target_include_directories(cimgui INTERFACE ${CIMGUI_PATH})


# set(SOKOL_FILES
#     "https://raw.githubusercontent.com/floooh/sokol/refs/heads/master/sokol_app.h|sokol_app.h"
#     "https://raw.githubusercontent.com/floooh/sokol/refs/heads/master/sokol_gfx.h|sokol_gfx.h"
#     "https://raw.githubusercontent.com/floooh/sokol/refs/heads/master/sokol_time.h|sokol_time.h"
#     # add more lines as needed
# )

# # Destination folder (inside the binary tree so it never pollutes source)
# set(SOKOL_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/include/sokol")
# file(MAKE_DIRECTORY "${SOKOL_DEST_DIR}")

# ----------------------------------------------------------------------
# Helper function – download one file (with optional hash)
# ----------------------------------------------------------------------
# function(download_sokol_file URL LOCAL_NAME)
#     set(DEST "${SOKOL_DEST_DIR}/${LOCAL_NAME}")

#     # ----> OPTIONAL: provide a SHA-256 hash here for integrity
#     set(EXPECTED_HASH "")                     # <-- leave empty to skip check
#     # Example (uncomment & fill):
#     # set(EXPECTED_HASH "SHA256=ab12…")

#     file(DOWNLOAD
#          "${URL}"
#          "${DEST}"
#          STATUS _status
#          ${EXPECTED_HASH}                     # omitted if empty
#          SHOW_PROGRESS
#          TLS_VERIFY ON
#          TIMEOUT 30)                         # abort after 30 s

#     list(GET _status 0 _code)
#     if(NOT _code EQUAL 0)
#         list(GET _status 1 _msg)
#         message(FATAL_ERROR "Download failed: ${URL} → ${_msg}")
#     endif()

#     # Make CMake re-configure if the remote file changes
#     # (only works when a hash is *not* supplied – CMake then stores a timestamp)
#     if(NOT EXPECTED_HASH)
#         set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${URL}")
#     endif()
# endfunction()

# ----------------------------------------------------------------------
# Perform the downloads
# ----------------------------------------------------------------------
# foreach(entry IN LISTS SOKOL_FILES)
#     string(REPLACE "|" ";" parts "${entry}")
#     list(GET parts 0 URL)
#     list(GET parts 1 LOCAL_NAME)
#     download_sokol_file("${URL}" "${LOCAL_NAME}")
# endforeach()


# #=== LIBRARY: sokol
# set(SOKOL_PATH_DIR libs/sokol)
# # add headers to the the file list because they are useful to have in IDEs
# set(SOKOL_HEADERS
#     ${SOKOL_PATH_DIR}/sokol_gfx.h
#     ${SOKOL_PATH_DIR}/sokol_app.h
#     ${SOKOL_PATH_DIR}/sokol_imgui.h
#     ${SOKOL_PATH_DIR}/sokol_glue.h)
# if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
#     add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
#     target_compile_options(sokol PRIVATE -x objective-c)
#     target_link_libraries(sokol PUBLIC
#         "-framework QuartzCore"
#         "-framework Cocoa"
#         "-framework MetalKit"
#         "-framework Metal")
# else()
#     add_library(sokol STATIC ${SOKOL_PATH_DIR}/sokol.c ${SOKOL_HEADERS})
#     if (CMAKE_SYSTEM_NAME STREQUAL Linux)
#         target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL dl m)
#         target_link_libraries(sokol PUBLIC Threads::Threads)
#     endif()
# endif()
# target_link_libraries(sokol PUBLIC cimgui)
# target_include_directories(sokol INTERFACE ${SOKOL_PATH_DIR})


# set(STB_PATH libs/stb)

# set(LIBS_INCLUDE_DIR libs)

# set(SRC_FILES
#     ${LIBS_INCLUDE_DIR}/nuklear/nuklear.c
# )

# set(APP_NAME demo)
# set(MAIN_FILE
#     ${SRC_FILES}
#     # src/demo.c
#     # src/sokol_flecs.c
#     # examples/sokol_window.c
#     # examples/triangle.c
#     # examples/quad.c
#     # examples/quad_wireframe.c
#     # examples/quad_index.c
#     # examples/sokol_input.c
#     # examples/sokol_quad_texture.c
#     examples/sokol_nuklear.c
# )
# #=== EXECUTABLE: demo
# if(CMAKE_SYSTEM_NAME STREQUAL Windows)
#     add_executable(${APP_NAME} WIN32 ${MAIN_FILE})
#     set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT demo)
# else()
#     add_executable(${APP_NAME} ${MAIN_FILE})
# endif()
# target_link_libraries(${APP_NAME} 
#     sokol
#     # flecs                                           # flecs
# )
# target_include_directories(${APP_NAME} PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
#     ${STB_PATH}
#     ${LIBS_INCLUDE_DIR}
# )
# # this hack removes the xxx-CMakeForceLinker.cxx dummy file
# set_target_properties(${APP_NAME} PROPERTIES LINKER_LANGUAGE C)








# set(APP_NAME demo)
# set(MAIN_FILE
#     # src/demo.c
#     # src/sokol_flecs.c
#     src/sokol_window.c
#     # src/triangle.c
# )
# #=== EXECUTABLE: demo
# if(CMAKE_SYSTEM_NAME STREQUAL Windows)
#     add_executable(${APP_NAME} WIN32 ${MAIN_FILE})
#     set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT demo)
# else()
#     add_executable(${APP_NAME} ${MAIN_FILE})
# endif()
# target_link_libraries(${APP_NAME} 
#     sokol
#     # flecs                                           # flecs
# )
# target_include_directories(${APP_NAME} PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )
# # Emscripten-specific linker options
# if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
#     set(CMAKE_EXECUTABLE_SUFFIX ".html")
#     # use our own minimal shell.html
#     target_link_options(${APP_NAME} PRIVATE --shell-file ../sokol/shell.html)
#     # link with WebGL2
#     target_link_options(${APP_NAME} PRIVATE -sUSE_WEBGL2=1)
#     # WASM+JS size optimizations
#     target_link_options(${APP_NAME} PRIVATE -sNO_FILESYSTEM=1 -sASSERTIONS=0 -sMALLOC=emmalloc --closure=1)
# endif()
# # explicitly strip dead code
# if (CMAKE_C_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten)
#     target_link_options(${APP_NAME} PRIVATE LINKER:-dead_strip)
# endif()
# # this hack removes the xxx-CMakeForceLinker.cxx dummy file
# set_target_properties(${APP_NAME} PROPERTIES LINKER_LANGUAGE C)



