/*
    Rectangle rendering is not supported yet.
    However the current implementation for Sprites does not render textures at the moment.
    Thus to showcase the rendering, we can piggyback of the DrawSprite call to render a rectangle.
*/

#include <chrono>
#include <filesystem>
#include <string>

#include "ienium/glow/core/Vector.hpp"
#include "ienium/glow/core/publictypes.hpp"
#include "ienium/utils/color/ieniumcolor.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GL/glew.h"
#include "ienium/glow/glow.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

using namespace ienium;
using namespace ienium::utils;
using namespace ienium::glow;

void errorCallback(int error, const char* description)
{
    LoggerManager::GetInstance().GetLogger()->Log(IENIUM_ERROR, "GLWF3 An error occured: " + std::to_string(error) + "\n" + description);
}

int main()
{
    LoggerManager::GetInstance().SetLogger<ConsoleLogger>();
    auto* logger = LoggerManager::GetInstance().GetLogger();

    if (!glfwInit())
    {
        logger->Log(IENIUM_ERROR, "Initialization of glfw failed!");
        return -1;
    }
    logger->Log(IENIUM_INFO, "GLFW3 | Initialized.");

    glfwSetErrorCallback(errorCallback);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Render Window", nullptr, nullptr);

    if (!window)
    {
        logger->Log(IENIUM_ERROR, "Window or OpenGL context creation failed!");
        return -1;
    }
    logger->Log(IENIUM_INFO, "GLFW3 | Window initialized.");

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    GLenum err = glewInit();

    if (err != GLEW_OK)
    {
        logger->Log(IENIUM_ERROR, "Initialization of glew failed!");
        return -1;
    }
    logger->Log(IENIUM_INFO, "Glew | Initialized.");

    Renderer2D renderer;
    if (!renderer.Initialize())
    {
        logger->Log(IENIUM_ERROR, "Initialization of renderer failed!");
        return -1;
    }
    logger->Log(IENIUM_INFO, "Glow | Initialized.");

    auto program_id = loadShaders("examples/basic_triangle/resources/vertexshader.glsl", "examples/basic_triangle/resources/fragmentshader.glsl");

    logger->Log(IENIUM_INFO, "GLFW3 | Starting window loop.");

    auto last_time = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program_id);

        renderer.BeginFrame();

        /*  Rectangle rendering is currently not supported directly.
            However the current implementation for Sprites does not render textures at the moment.
            Thus to showcase the rendering, we can piggyback of the DrawSprite call
        */

        Vector2    size(0.5, 0.5);                // Size of the Rectangle (currently in screen space (so percentage of the screen size))
        Vector2    position(0, 0);                // Position of the rectangle (anchord at the rectengle center) (currently in screen space)
        ResourceId texture_id = INVALID_RESOURCE; // Irrelevant for rectangle rendering
        Color      color(0.6F, 0.3F, 1.0F);       // Color of the rectangle
        Vector2    texture_scale(0, 0);           // Irrelevant for rectangle rendering
        Vector2    texture_offset(0, 0);          // Rotation of the rectangle as angle in radians (Currently nor implemented)
        float      angle = 0;

        renderer.DrawSprite(size, position, texture_id, color, texture_scale, texture_offset, angle);

        renderer.EndFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();

        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - last_time).count();
        // LOGGER->Log(ienium::utils::IENIUM_DEBUG, "Frame time: " + std::to_string(diff) + "us");
        last_time = std::chrono::system_clock::now();
    }
    renderer.Shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    logger->Log(IENIUM_INFO, "GLFW3 | Shutdown gracefully");
}