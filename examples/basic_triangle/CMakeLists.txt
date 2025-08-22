add_executable(basic_triangle main.cpp)

target_link_libraries(basic_triangle
    PRIVATE
        Ienium::Glow
)

set_target_properties(basic_triangle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/basic_triangle
)

# Find all DLLs for MyApp's dependencies
add_custom_command(TARGET basic_triangle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:basic_triangle>     # Collects all DLLs
        $<TARGET_FILE_DIR:basic_triangle>         # Destination (same as exe)
    COMMAND_EXPAND_LISTS                        # Needed for DLL list expansion
)