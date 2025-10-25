add_executable(dev_proj main.cpp)

target_link_libraries(dev_proj
    PRIVATE
        Ienium::Glow
        glfw
)

set_target_properties(dev_proj PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/dev_proj
)

if (WIN32)
    add_custom_command(TARGET dev_proj POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:dev_proj>     # Collects all DLLs
            $<TARGET_FILE_DIR:dev_proj>         # Destination (same as exe)
        COMMAND_EXPAND_LISTS                        # Needed for DLL list expansion
    )
endif()