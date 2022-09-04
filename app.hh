#ifndef APP_H
#define APP_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hh"

class Application
{
private:
    GLFWwindow *window;
    std::vector<Shader> shaders;

    static void GLAPIENTRY error_callback(GLenum source,
                                          GLenum type,
                                          GLuint id,
                                          GLenum severity,
                                          GLsizei length,
                                          const GLchar *message,
                                          const void *userParam)
    {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type, severity, message);
    }

    void m_setup()
    {
        Setup();
    }

    void m_init()
    {
        if (!glfwInit())
            Kill("Failed: glfwInit");
        window = glfwCreateWindow(width, height, "", NULL, NULL);
        if (!window)
            Kill("Failed: glfwCreateWindow");
        glfwMakeContextCurrent(window);
        if (glewInit() != GLEW_OK)
            Kill("Failed: glewInit");
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(error_callback, 0);
        Init();
        return;
    }

    void m_input()
    {
        Input();
    }

    void m_update()
    {
        Update();
    }

    void m_render()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
protected:
    uint width, height;
public:
    // Non-OpenGL Setup 
    // OpenGL functions have not been loaded, and so won't work
    // Defer any OpenGL setup to Init()
    virtual void Setup() = 0;

    // OpenGL Related Setup
    // OpenGL functions have been loaded, and will work
    virtual void Init() = 0;

    virtual void Input() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    void Start()
    {
        m_setup();
        m_init();
        while (isRunning())
        {
            m_input();
            m_update();
            m_render();
        }
    }

    bool isRunning()
    {
        return !glfwWindowShouldClose(window);
    }

    // Acts as a panic button, will throw message and exit without any cleanup
    // Beware memory leaks
    void Kill(std::string message)
    {
        std::cout << message << std::endl;
        std::exit(-1);
    }
};

#endif