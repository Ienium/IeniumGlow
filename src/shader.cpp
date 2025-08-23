#include "GL/glew.h"
#include "ienium/utils/logger/ieniumlogger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "ienium/glow/glow.hpp"

namespace ienium
{
    GLuint LoadShaders (const std::filesystem::path& vertex_file_path, const std::filesystem::path& fragment_file_path)
    {
        std::string vertex_file_path_str = std::filesystem::absolute(vertex_file_path).string ();
        std::string fragment_file_path_str = std::filesystem::absolute(fragment_file_path).string ();

        auto logger = utils::LoggerManager::GetInstance ().GetLogger ();
        // Create the shaders
        GLuint vertex_shader_id = glCreateShader (GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader (GL_FRAGMENT_SHADER);

        // Read the vertex shader code from the file
        std::string vertex_shader_code;
        std::ifstream vertex_shader_stream (vertex_file_path_str, std::ios::in);
        if (vertex_shader_stream.is_open ())
        {
            std::stringstream ss;
            ss << vertex_shader_stream.rdbuf ();
            vertex_shader_code = ss.str ();
            vertex_shader_stream.close ();
        }
        else
        {
            logger->Log (utils::IENIUM_ERROR, "Can not open vertex shader path: " + vertex_file_path_str + ". Are you in the right directory?");
            return 0;
        }

        // Read the fragment shader code from the file
        std::string fragment_shader_code;
        std::ifstream fragment_shader_stream (fragment_file_path_str, std::ios::in);
        if (fragment_shader_stream.is_open ())
        {
            std::stringstream ss;
            ss << fragment_shader_stream.rdbuf ();
            fragment_shader_code = ss.str ();
            fragment_shader_stream.close ();
        }
        else
        {
            logger->Log (utils::IENIUM_ERROR, "Can not open fragment shader path: " + fragment_file_path_str + ". Are you in the right directory?");
            return 0;
        }
        
        GLint result = GL_FALSE;
        int info_log_length;

        // Compile vertex shader
        logger->Log (utils::IENIUM_INFO, "Compiling vertex shader: " + vertex_file_path_str);
        char const * vertex_code_ptr = vertex_shader_code.c_str ();
        glShaderSource (vertex_shader_id, 1, &vertex_code_ptr, NULL);
        glCompileShader (vertex_shader_id);

        // Check vertex shader
        glGetShaderiv (vertex_shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv (vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        if (info_log_length > 0)
        {
            char* vertex_shader_error_message = new char [info_log_length + 1];
            glGetShaderInfoLog (vertex_shader_id, info_log_length, NULL, &vertex_shader_error_message[0]);
            logger->Log(utils::IENIUM_ERROR, "Error compiling vertex shader: " + vertex_file_path_str + ":\n" + vertex_shader_error_message);
            delete[] vertex_shader_error_message;
        }

        // Compile fragment shader
        logger->Log (utils::IENIUM_INFO, "Compiling fragment shader: " + fragment_file_path_str);
        char const * fragment_code_ptr = fragment_shader_code.c_str ();
        glShaderSource (fragment_shader_id, 1, &fragment_code_ptr, NULL);
        glCompileShader (fragment_shader_id);

        // Check fragment shader
        glGetShaderiv (fragment_shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv (fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        if (info_log_length > 0)
        {
            char* fragment_shader_error_message = new char [info_log_length + 1];
            glGetShaderInfoLog (fragment_shader_id, info_log_length, NULL, &fragment_shader_error_message[0]);
            logger->Log(utils::IENIUM_ERROR, "Error compiling fragment shader: " + fragment_file_path_str + ":\n" + fragment_shader_error_message);
            delete[] fragment_shader_error_message;
        }

        // Link program
        logger->Log(utils::IENIUM_INFO, "Linking shader program.");
        GLuint program_id = glCreateProgram ();
        glAttachShader (program_id, vertex_shader_id);
        glAttachShader (program_id, fragment_shader_id);
        glLinkProgram (program_id);

        // Check program
        glGetProgramiv (program_id, GL_LINK_STATUS, &result);
        glGetProgramiv (program_id, GL_INFO_LOG_LENGTH, &info_log_length);

        if (info_log_length > 0)
        {
            char* program_error_message = new char [info_log_length + 1];
            glGetProgramInfoLog (program_id, info_log_length, NULL, &program_error_message[0]);
            logger->Log(utils::IENIUM_ERROR, std::string ("Error compiling program:\n") + program_error_message);
            delete[] program_error_message;
        }

        // Cleanup
        glDetachShader (program_id, vertex_shader_id);
        glDetachShader (program_id, fragment_shader_id);

        glDeleteShader (vertex_shader_id);
        glDeleteShader (fragment_shader_id);

        return program_id;
    }
}

