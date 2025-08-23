#pragma once

// Acts as public API only include what is publicly necessary
#include "renderer2d.hpp"
#include "camera_2d.hpp"
#include "sprite.hpp"
#include "font.hpp"
#include <filesystem>

// Not neccessary modules:
// #include "batch_renderer.hpp"


// namespace function declaration

namespace ienium
{
    unsigned int LoadShaders (const std::filesystem::path& vertex_file_path, const std::filesystem::path& fragment_file_path);
}
