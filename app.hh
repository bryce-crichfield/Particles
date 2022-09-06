#ifndef APP_H
#define APP_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hh"
#include "util.hh"

class Application
{
private:
    GLFWwindow *window;
    // OpenGL Debug Message
    static void GLAPIENTRY error_callback(GLenum source,
                                          GLenum type,
                                          GLuint id,
                                          GLenum severity,
                                          GLsizei length,
                                          const GLchar *message,
                                          const void *userParam)
    {
        if (false)
            fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                    type, severity, message);
    }

    // Framework related setup, called by Application::Start()
    void m_setup()
    {
        Setup();
    }
    // Framework related intialization, called by Application::Start()
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
    // Framework related input handling, called by Application::Start()
    void m_input()
    {
        Input();
    }
    // Framework related update, called by Application::Start()
    void m_update(float delta_t)
    {
        Update(delta_t);
    }
    // Framework related render, called by Application::Start()
    void m_render(float delta_t)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render(delta_t);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

protected:
    uint width, height;
    uint fps, ups;

public:
    // Non-OpenGL Setup
    // OpenGL functions have not been loaded, and so won't work
    // Defer any OpenGL setup to Init()
    virtual void Setup() = 0;
    // OpenGL Related Setup
    // OpenGL functions have been loaded, and will work
    virtual void Init() = 0;
    virtual void Input() = 0;
    virtual void Update(float delta) = 0;
    virtual void Render(float delta) = 0;
    void Start()
    {
        m_setup();
        m_init();
        uint64_t last_update = time();
        uint64_t last_render = time();
        uint64_t delta_update = 0;
        uint64_t delta_render = 0;
        uint64_t accum_update = 0;
        uint64_t accum_render = 0;
        while (isRunning())
        {
            uint64_t now = time();
            delta_update = now - last_update;
            last_update = now;
            delta_render = now - last_render;
            delta_render = now;
            accum_update += delta_update;
            accum_render += delta_render;
            m_input();
            if (accum_update >= (uint64_t)1000/ups)
            {
                m_update(accum_update * .001f);
                accum_update = 0;
            }
            if (accum_render >= (uint64_t)1000/fps)
            {
                m_render(accum_render * .001f);
                accum_render = 0;
            }
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