add_executable(blend_enum_test main.cpp)

target_link_libraries(blend_enum_test
    PRIVATE
        Ienium::Glow
        GLEW::GLEW
        glfw
)

set_target_properties(blend_enum_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/blend_enum_test
)


add_custom_command(TARGET blend_enum_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:blend_enum_test>
        $<TARGET_FILE_DIR:blend_enum_test>
    COMMAND_EXPAND_LISTS
)

add_test(NAME BlendEnumTest COMMAND blend_enum_test)