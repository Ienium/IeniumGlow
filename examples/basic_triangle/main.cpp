#include "ienium/glow/core/Vector.hpp"
#include "ienium/glow/core/internaldefinitions.hpp"
#include "ienium/utils/color/ieniumcolor.hpp"
#include <chrono>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "GL/glew.h"

#include "ienium/glow/glow.hpp"
#include "ienium/glow/vertexbuffermanager.hpp"
//#include "ienium/glow/renderer2d.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

using namespace ienium;
using namespace ienium::utils;
using namespace ienium::glow;

void error_callback (int error, const char* description)
{
    LoggerManager::GetInstance().GetLogger()->Log(IENIUM_ERROR, "GLWF3 An error occured: " + std::to_string(error) + "\n" + description);
}

int main ()
{
    LoggerManager::GetInstance().SetLogger<ConsoleLogger> ();
    auto logger = LoggerManager::GetInstance().GetLogger();
    if (!glfwInit ())
    {
        logger->Log(IENIUM_ERROR, "Initialization of glfw failed!");
        return -1;
    }
    logger->Log(IENIUM_INFO, "GLFW3 | Initialized.");
       
    
    glfwSetErrorCallback (error_callback);

    GLFWwindow* window = glfwCreateWindow (1280, 720, "Render Window", NULL, NULL);

    if (!window)
    {
        logger->Log(IENIUM_ERROR, "Window or OpenGL context creation failed!");
        return -1;
    }
    logger->Log(IENIUM_INFO, "GLFW3 | Window initialized.");
        

    glfwMakeContextCurrent (window);
    glfwSwapInterval(0);
    GLenum err = glewInit ();
    if (err != GLEW_OK)
    {
        logger->Log(IENIUM_ERROR, "Initialization of glew failed!");
        return -1;
    }
    logger->Log(IENIUM_INFO, "Glew | Initialized.");

    Renderer2D renderer;
    if (!renderer.Initialize ())
    {
        logger->Log(IENIUM_ERROR, "Initialization of renderer failed!");
        return -1;
    }
    logger->Log(IENIUM_INFO, "Glow | Initialized.");

    auto program_id = LoadShaders("examples\\basic_triangle\\resources\\vertexshader.glsl", "examples\\basic_triangle\\resources\\fragmentshader.glsl");

    logger->Log(IENIUM_INFO, "GLFW3 | Starting window loop.");    

    std::chrono::system_clock clock;
    auto last_time = clock.now ();
    while (!glfwWindowShouldClose (window) && !glfwGetKey(window, GLFW_KEY_ESCAPE ))
    {
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram (program_id);
        
        renderer.BeginFrame ();

        for (int i = 0; i < 10000; i++)
        {
            renderer.SetLayer(0);
            renderer.DrawSprite(Vector2(0.5,0.5), Vector2(0.5,0.5), 0);
            renderer.SetLayer(1);
            renderer.DrawSprite(Vector2(0.5,0.5), Vector2(-0.5,-0.5), 0);
            renderer.SetLayer(2);
            renderer.DrawSprite(Vector2(0.5,0.5), Vector2(-0.5,0.5), 0);
            renderer.SetLayer(3);
            renderer.DrawSprite(Vector2(0.5,0.5), Vector2(0.5,-0.5), 0);
        }
        
        //renderer.DrawSprite(Vector2(0.5,0.5), Vector2(-0.5,-0.5), 0);
        
        renderer.EndFrame ();

        glfwSwapBuffers (window);
        glfwPollEvents ();

        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(clock.now() - last_time).count();
        LOGGER->Log(ienium::utils::IENIUM_DEBUG, "Frame time: " + std::to_string(diff) + "ms");
        last_time = clock.now ();
    }

    glfwDestroyWindow (window);
    glfwTerminate ();

    logger->Log(IENIUM_INFO, "GLFW3 | Shutdown gracefully");
}