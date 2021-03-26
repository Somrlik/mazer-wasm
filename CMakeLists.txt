project (mazer)
cmake_minimum_required(VERSION 3.5.1)
option(DEBUG "Adds debugging features" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(include)

file(GLOB SOURCES src/*.cc)

set(CMAKE_BINARY_DIR ../public)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if(DEBUG)
    message(STATUS "Turning debugging features on")
    set(DEBUG_FLAGS "-g4 --source-map-base /")
    # set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USE_FLAGS "-s USE_SDL=2 -s USE_FREETYPE=1 -s USE_SDL_TTF=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[png] --preload-file ../assets/preload")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
else()
    find_package(SDL2 REQUIRED)
    find_package(Freetype REQUIRED)
endif()
include_directories(${CMAKE_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

# Declare more exported functions here
set(EXPORTED_FUNCTIONS "[\
    _main, \
    _js_on_resize_callback \
]")
string(REGEX REPLACE "[ \t\r\n]" "" EXPORTED_FUNCTIONS "${EXPORTED_FUNCTIONS}")

add_executable(mazer-fallback ${SOURCES})
set_target_properties(mazer-fallback PROPERTIES LINK_FLAGS "-s WASM=0 -s EXPORTED_FUNCTIONS='${EXPORTED_FUNCTIONS}'")
target_link_libraries(mazer-fallback ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

add_executable(mazer ${SOURCES})
set(WASM_CXX_FLAGS "${PROPERTIES} ${DEBUG_FLAGS}")
set_target_properties(mazer PROPERTIES LINK_FLAGS "-s WASM=1 -s EXPORTED_FUNCTIONS='${EXPORTED_FUNCTIONS}'" CMAKE_CXX_FLAGS "${WASM_CXX_FLAGS}")
target_link_libraries(mazer ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

add_custom_command(TARGET mazer POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/patchBuildFiles.js mazer
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    COMMENT "Patching files post-build"
)

add_custom_command(TARGET mazer-fallback POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/patchBuildFiles.js mazer-fallback
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    COMMENT "Patching files post-build"
)
