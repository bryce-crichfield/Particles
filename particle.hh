#ifndef PARTICLE_HH
#define PARTICLE_HH

#include <iostream>
#include <array>

#include "util.hh"
#include "shader.hh"

template <uint SIZE, class TYPE>
struct DoubleBuffer;

template <uint SIZE, class TYPE>
struct ParticleBuffer;
struct Particle;



template <uint SIZE, class TYPE>
struct DoubleBuffer
{
    std::array<TYPE, SIZE> *front;
    std::array<TYPE, SIZE> *back;
    std::array<TYPE, SIZE> b1;
    std::array<TYPE, SIZE> b2;
    DoubleBuffer() : front(&b1), back(&b2) { }
};


template <uint SIZE, class TYPE>
struct ParticleBuffer
{
    DoubleBuffer<SIZE, TYPE> buffer;

    void Update() 
    {
        // Read from Back into Front
        for (uint i = 0; i < SIZE; i++) {
            for (uint j = 0; j < SIZE; j++) {

            }

        }
    }

    void Render(Shader& shader)
    {
        // Using the provided shader, write the contents of the front buffer
        // To the screen, then perform a buffer swap

        // Buffer Swap
        auto temp = buffer.front;
        buffer.front = buffer.back;
        buffer.back = temp;
    }
};

struct Particle
{
    float x, y, vx, vy, dx, dy;

    Particle()
    {
        
    }

    static Particle RandomParticle() 
    {
        Particle out;
        out.x = random_between(-1.0f,1.0f);
        out.y = random_between(-1.0f, 1.0f);
        out.vx = 0;
        out.vy = 0;
        // out.vx = random_between(-.02,.2);
        // out.vy = random_between(-.2,.2);
        return out;
    }
};


#endif