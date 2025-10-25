#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <string>

#include "ienium/glow/core/Vector.hpp"
#include "ienium/glow/core/internaldefinitions.hpp"
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

        for (int i = 0; i < 100000; i++)
        {
            srand(i);
            float pos_x  = (rand() % 100 / 75.F) - 0.75;
            float pos_y  = (rand() % 100 / 75.F) - 0.75;
            float width  = rand() % 100 / 200.F;
            float height = rand() % 100 / 200.F;

            float red   = rand() % 100 / 100.F;
            float green = rand() % 100 / 100.F;
            float blue  = rand() % 100 / 100.F;

            int layer = (int)(rand() % 100 / 100.F * 4);

            // renderer.SetLayer(layer);
            renderer.DrawSprite(Vector2(width, height), Vector2(pos_x, pos_y), 0, utils::Color(red, green, blue));
        }

        // renderer.DrawSprite(Vector2(0.5,0.5), Vector2(-0.5,-0.5), 0);

        renderer.EndFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();

        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - last_time).count();
        LOGGER->Log(ienium::utils::IENIUM_DEBUG, "Frame time: " + std::to_string(diff) + "us");
        last_time = std::chrono::system_clock::now();
        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    renderer.Shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    logger->Log(IENIUM_INFO, "GLFW3 | Shutdown gracefully");
}