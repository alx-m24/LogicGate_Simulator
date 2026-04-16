cmake_minimum_required(VERSION 3.25)

project(LogicGate_Simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Dependencies ---
include(FetchContent)

# My custom engine
FetchContent_Declare(
    LexviEngine
    GIT_REPOSITORY https://github.com/alx-m24/LexviEngine.git
    GIT_TAG        old-engine
)

FetchContent_MakeAvailable(LexviEngine)

# --- Executable ----
add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    LexviEngine
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${LexviEngine_SOURCE_DIR}
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
endif()
