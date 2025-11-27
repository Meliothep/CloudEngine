project(RendererTest LANGUAGES CXX)

# --- Shader compile + copy support -----------------------------------------
# Place GLSL/HLSL shader source files in `apps/RendererTest/shaders/`.
# At configure/build time we compile them to SPIR-V and place them next to
# the executable in `.../shaders/` so the runtime can load them relative to the exe.

find_program(GLSLANG_VALIDATOR NAMES glslangValidator)
if(NOT GLSLANG_VALIDATOR)
    message(WARNING "glslangValidator not found; shader automatic compilation will be skipped.")
endif()

file(GLOB_RECURSE RENDERERTEST_SHADERS
         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.comp"
         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.geom"
)

set(SPIRV_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})

set(SPIRV_TARGETS)
foreach(SHADER ${RENDERERTEST_SHADERS})
    # Use the full filename (including original extension) to avoid collisions
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    # Output file: include original extension, then add .spv
    set(SPV_OUT "${SPIRV_OUTPUT_DIR}/${SHADER_NAME}.spv")
    if(GLSLANG_VALIDATOR)
        add_custom_command(
            OUTPUT ${SPV_OUT}
            COMMAND ${GLSLANG_VALIDATOR} -V "${SHADER}" -o "${SPV_OUT}"
            DEPENDS "${SHADER}"
            COMMENT "Compiling shader ${SHADER} -> ${SPV_OUT}"
            VERBATIM
        )
        list(APPEND SPIRV_TARGETS ${SPV_OUT})
    endif()
endforeach()

if(SPIRV_TARGETS)
    add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_TARGETS})
else()
    if(GLSLANG_VALIDATOR)
        message(WARNING "No shader sources found in ${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
    endif()
endif()

# Add executable and wire shader build/copy
add_executable(RendererTest src/main.cpp)

target_link_libraries(RendererTest
        PRIVATE VulkanRenderer
)

# Ensure shaders are compiled before the executable is linked/built
if(TARGET CompileShaders)
    add_dependencies(RendererTest CompileShaders)
    # After build, copy compiled SPIR-V files next to the executable in a `shaders/` folder
    add_custom_command(TARGET RendererTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:RendererTest>/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SPIRV_OUTPUT_DIR}" "$<TARGET_FILE_DIR:RendererTest>/shaders"
        COMMENT "Copying compiled shaders next to RendererTest executable"
    )
endif()
