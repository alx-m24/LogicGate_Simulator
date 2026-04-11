cmake_minimum_required(VERSION 3.16)
project(LogicGate_Simulator LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Source files
# ----------------------------
set(SOURCES
    src/main.cpp
    src/Headers/CircuitFileHandler.cpp
    src/Headers/Gate.cpp
    src/Headers/gui/nfd_common.c
    src/Headers/Node.cpp
    src/Headers/Simulation.cpp
    src/Headers/Wire.cpp
)

if(WIN32)
    list(APPEND SOURCES src/Headers/gui/nfd_win.cpp)
endif()

add_executable(LogicGate_Simulator ${SOURCES})

target_include_directories(LogicGate_Simulator PRIVATE
    src/Headers
    src/Headers/gui
)

target_compile_definitions(LogicGate_Simulator PRIVATE _CRT_SECURE_NO_WARNINGS)

# ----------------------------
# Dependencies
# ----------------------------
include(FetchContent)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

# ----------------------------
# JSON
# ----------------------------
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# ----------------------------
# SFML (static)
# ----------------------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_STATIC_LIBRARIES ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

# ----------------------------
# ImGui + ImGui-SFML (static)
# ----------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.9
)
FetchContent_MakeAvailable(imgui)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
target_include_directories(LogicGate_Simulator PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/misc/cpp
)

FetchContent_Declare(
    imgui_sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v2.6
)
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(imgui_sfml)

# ----------------------------
# curl (shared is fine)
# ----------------------------
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-8_3_0
)
FetchContent_MakeAvailable(curl)

# ----------------------------
# Target linking
# ----------------------------
target_link_libraries(LogicGate_Simulator PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    nlohmann_json::nlohmann_json
    ImGui-SFML::ImGui-SFML
    CURL::libcurl
    -lucrtbase
)

# ----------------------------
# Windows runtime (MinGW only)
# ----------------------------
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    foreach(dll libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=${dll}
            OUTPUT_VARIABLE DLL_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(EXISTS ${DLL_PATH})
            add_custom_command(TARGET LogicGate_Simulator POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DLL_PATH}"
                $<TARGET_FILE_DIR:LogicGate_Simulator>
            )
        endif()
    endforeach()
endif()

# ----------------------------
# Copy assets to build output
# ----------------------------
add_custom_command(TARGET LogicGate_Simulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/Logic Gate.ico"
        $<TARGET_FILE_DIR:LogicGate_Simulator>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/Logic Gate.png"
        $<TARGET_FILE_DIR:LogicGate_Simulator>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/Shaders"
        "$<TARGET_FILE_DIR:LogicGate_Simulator>/src/Shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:LogicGate_Simulator>/Saves"
)
