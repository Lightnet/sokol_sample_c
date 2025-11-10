cmake_minimum_required(VERSION 3.20)
project(sokol-sample)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)

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


#=== LIBRARY: cimgui + Dear ImGui
add_library(cimgui STATIC
    cimgui/cimgui.cpp
    cimgui/cimgui.h
    cimgui/imgui.cpp
    cimgui/imgui.h
    cimgui/imgui_widgets.cpp
    cimgui/imgui_draw.cpp
    cimgui/imgui_tables.cpp
    cimgui/imgui_demo.cpp
)
target_include_directories(cimgui INTERFACE cimgui)

#=== LIBRARY: sokol
# add headers to the the file list because they are useful to have in IDEs
set(SOKOL_HEADERS
    sokol/sokol_gfx.h
    sokol/sokol_app.h
    sokol/sokol_imgui.h
    sokol/sokol_glue.h)
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
    target_compile_options(sokol PRIVATE -x objective-c)
    target_link_libraries(sokol PUBLIC
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework MetalKit"
        "-framework Metal")
else()
    add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
    if (CMAKE_SYSTEM_NAME STREQUAL Linux)
        target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL dl m)
        target_link_libraries(sokol PUBLIC Threads::Threads)
    endif()
endif()
target_link_libraries(sokol PUBLIC cimgui)
target_include_directories(sokol INTERFACE sokol)







set(APP_NAME demo)
set(MAIN_FILE
    # src/demo.c
    # src/sokol_flecs.c
    src/sokol_window.c
    # src/triangle.c
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
)
# this hack removes the xxx-CMakeForceLinker.cxx dummy file
set_target_properties(${APP_NAME} PROPERTIES LINKER_LANGUAGE C)








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



