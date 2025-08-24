# include "ienium/glow/renderer2d.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

#include <GL/glew.h>
#include <exception>
#include <memory>

using namespace ienium::utils;

namespace ienium::glow
{
    class Renderer2D::Impl
    {
        public:
            GLuint shaderProgram;
            GLuint vertexBuffer;
            GLuint vertexArray;

            void DrawTriangle ()
            {
                try {
                {
                    glGenVertexArrays (1, &vertexArray);
                    glBindVertexArray (vertexArray);

                    static const GLfloat g_vertex_buffer_data[] = {
                        -1.f,   -1.f,   0.f,
                        1.f,    -1.f,   0.f,
                        0.f,    1.f,    0.f
                    };

                    glGenBuffers (1, &vertexBuffer);
                    glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
                    glBufferData (GL_ARRAY_BUFFER, sizeof (g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

                    glEnableVertexAttribArray (0);
                    glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
                    glVertexAttribPointer (
                        0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        (void*)0
                    );

                    glDrawArrays (GL_TRIANGLES, 0, 3);
                    glDisableVertexAttribArray (0);
                }
                } catch (std::exception e)
                {
                    auto err = glGetError ();
                    LoggerManager::GetInstance().GetLogger()->Log(IENIUM_ERROR, "Error drawing triangle: " + std::string((const char*)glewGetErrorString (err)));

                }
            }
    };

    Renderer2D::Renderer2D ()
    {

    }

    Renderer2D::~Renderer2D()
    {

    }

    bool Renderer2D::Initialize ()
    {
        impl = std::make_unique<Impl>();
        return true;
    }

    void Renderer2D::Shutdown ()
    {

    }

    void Renderer2D::Clear (const Color& color)
    {

    }

}