add_executable(vector_test main.cpp)

target_link_libraries(vector_test
    PRIVATE
        Ienium::Glow
        GLEW::GLEW
        glfw
)

set_target_properties(vector_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/vector_test
)

# Find all DLLs for MyApp's dependencies
add_custom_command(TARGET vector_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:vector_test>     # Collects all DLLs
        $<TARGET_FILE_DIR:vector_test>         # Destination (same as exe)
    COMMAND_EXPAND_LISTS                       # Needed for DLL list expansion
)

add_test(NAME Vector2Test COMMAND vector_test)