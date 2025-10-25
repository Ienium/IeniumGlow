add_executable(basic_rectangle main.cpp)

target_link_libraries(basic_rectangle
    PRIVATE
        Ienium::Glow
        glfw
)

set_target_properties(basic_rectangle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/basic_rectangle
)

if (WIN32)
    add_custom_command(TARGET basic_rectangle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:basic_rectangle>     # Collects all DLLs
            $<TARGET_FILE_DIR:basic_rectangle>         # Destination (same as exe)
        COMMAND_EXPAND_LISTS                        # Needed for DLL list expansion
    )
endif()