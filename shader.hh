#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <array>

#include "util.hh"
class Stage;
class Shader;

template <uint SIZE, class ENUM>
class Library;

class Stage
{
private:
    friend class Shader;
    GLuint shader;

    Stage() : shader(-1) { }

    Stage(const Stage& other)
        : shader(other.shader)
    { }

    Stage(const char* path, GLenum ShaderStage)
    { 
        auto file = read(path);
        if(file == nullptr) {
            std::cout << "Failed to read " << path << std::endl;
            return;
        }
        auto source = (char*) file;

        shader = glCreateShader(ShaderStage);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        

        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<char> log(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &log[0]);
            for (auto c : log) std::cout << c; 
            std::cout << path << std::endl;
            printf("Error : Stage Compile\n");
        }
    }

    ~Stage()
    {
        glDeleteShader(shader);
    }
};

struct Shader 
{
private:
    bool initialized;
    Stage& vertex;
    Stage& fragment;
    GLuint program;
public:

    Shader() : vertex(*(new Stage())), fragment(*(new Stage())), program(-1), initialized(false) { }
    
    Shader (const Shader& other)
        : vertex(other.vertex),
        fragment(other.fragment),
        program(other.program),
        initialized(true)
    {
        printf("Copy Constructor Called\n");
    }

    Shader& operator=(Shader& other)
    {
        // The default constructor cannot properly init the shader
        // Therefore we cannot actually delete it in that case
        // So we must check to see if it has actually been initialized
        // It would be wiser to embed this into the control flow, but idk how
        if (initialized) {
            glUseProgram(0);
            glDetachShader(program, vertex.shader);
            glDetachShader(program, fragment.shader);
            glDeleteProgram(program);
            delete(&vertex);
            delete(&fragment);
        }
        vertex = other.vertex;
        fragment = other.fragment;
        program = other.program;
        return *this;
    }
    Shader(const char* v_path, const char* f_path) :
        vertex(*(new Stage(v_path, GL_VERTEX_SHADER))), 
        fragment(*(new Stage(f_path, GL_FRAGMENT_SHADER))),
        initialized(false)
    {

        program = glCreateProgram();
        if(!program) printf("Error : glCreateProgram()\n");

        glAttachShader(program, vertex.shader);
        glAttachShader(program, fragment.shader);

        glBindAttribLocation(program, 0, "in_position");
        glBindAttribLocation(program, 1, "in_color");

        glLinkProgram(program);

        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if(linked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<char> log(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &log[0]);
            for (auto c : log) std::cout << c;
            printf("Error : Shader Link");
        }
        initialized = true;
    }

    ~Shader()
    {
        initialized = false;
        glUseProgram(0);
        glDetachShader(program, vertex.shader);
        glDetachShader(program, fragment.shader);
        glDeleteProgram(program);
        delete(&vertex);
        delete(&fragment);
    }

    void Use() {
        glUseProgram(program);
    }

    GLuint getProgramId() { return program; }
};

template <uint SIZE, class ENUM>
class Library
{
private:
    std::array<Shader, SIZE> shaders;

public:

    Library()
    {
        // shaders.fill(Shader());
    }

    void Load(ENUM ID, 
        const char* v_shader_path, 
        const char* f_shader_path)
    {
        Shader shader(v_shader_path, f_shader_path);
        shaders.data()[static_cast<uint>(ID)] = shader;
    }

    Shader& Get(ENUM ID)
    {
        auto out = shaders.data()[static_cast<uint>(ID)];
        return out;    
    }

};

#endif 