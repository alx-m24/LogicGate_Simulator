cmake_minimum_required(VERSION 3.25)

project(LogicGate_Simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Executable ----
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Globals.cpp
    src/Node/Node.cpp
    src/Simulation/Simulation.cpp
    src/Background/Background.cpp
)

# --- Dependencies ---
include(FetchContent)

# SFML
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# --- Copy GLFW and GLAD DLLs if they were built shared (Windows) ---
if(WIN32)
    foreach(_target glfw glad)
        if(TARGET ${_target})
            get_target_property(_target_type ${_target} TYPE)
            if(_target_type STREQUAL "SHARED_LIBRARY")
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:${_target}>
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>
                    COMMENT "Copying ${_target}.dll"
                )
            endif()
        endif()
    endforeach()
endif()

# --- Windows runtime (MinGW only) ---
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

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# --- Copy assests to build output ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/res"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
)
