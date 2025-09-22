set(SOURCES
    renderer2d.cpp
    shader.cpp
    vertexbuffermanager.cpp
    memorymanager.cpp
    batch_renderer.cpp
    core/Vector.cpp
)

add_library(ienium_glow STATIC ${SOURCES})
add_library(Ienium::Glow ALIAS ienium_glow)

target_include_directories(ienium_glow PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../External/IeniumUtils>
    $<INSTALL_INTERFACE:External/IeniumUtils>
)


target_link_libraries(ienium_glow PUBLIC
    GLEW::GLEW
    stdc++exp
)

# Compiler warnings
if(MSVC)
    target_compile_options(ienium_glow PRIVATE /W4)
else()
    target_compile_options(ienium_glow PRIVATE -Wall -Wextra -pedantic)
endif()

# Preprocessor definitions
target_compile_definitions(ienium_glow
    PUBLIC
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)