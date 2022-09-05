#ifndef PARTICLE_HH
#define PARTICLE_HH

#include <cstdlib>
#include <iostream>
#include <array>

#include <glm/vec3.hpp>

#include "util.hh"
#include "shader.hh"

template <uint SIZE>
struct ParticleBuffer;
struct Particle;

struct Particle
{
    glm::vec2 p, v, a;

    Particle() {}

    static Particle RandomParticle()
    {
        Particle out;
        out.p.x = random_between(-1.0f, 1.0f);
        out.p.y = random_between(-1.0f, 1.0f);
        // out.p.z = random_between(-1.0f, 1.0f);
        out.v.x = random_between(-.25, .25);
        out.v.y = random_between(-.25, .25);
        // out.v.z = random_between(-.25, .25);
        return out;
    }

    void bound(float x1, float x2, float y1, float y2, float z1, float z2, float jettison)
    {
        if (p.x <= x1)
        {
            v.x *= -jettison;
            p.x = x1;
        }
        if (p.x >= x2)
        {
            v.x *= -jettison;
            p.x = x2;
        }
        if (p.y <= y1)
        {
            v.y *= -jettison;
            p.y = y1;
        }
        if (p.y >= y2)
        {
            v.y *= -jettison;
            p.y = y2;
        }
    }
};

template <uint SIZE>
struct ParticleBuffer
{
    std::array<Particle, SIZE> buffer;

    ParticleBuffer()
    {
        for (uint i = 0; i < SIZE; i++)
        {
            buffer.at(i) = Particle::RandomParticle();
        }
    }

    template <uint S2>
    void Interact(ParticleBuffer<S2> &that, float G, float Min_R, float Max_R, float delta)
    {
        #pragma omp parallel for
        for (uint i = 0; i < SIZE; i++)
        {
            Particle &p1 = buffer.at(i);
            p1.a = glm::vec3(0);
            for (uint j = 0; j < S2; j++)
            {
                Particle &p2 = that.buffer.at(j);
                auto l = p1.p - p2.p;
                auto r = glm::length(l);
                if (r != 0 && r > Min_R && r < Max_R)
                {
                    p1.a = p1.a + (l / r);
                }
            }
            p1.v += p1.a * -G * delta;
            p1.p += p1.v * delta;
            p1.bound(-1, 1, -1, 1, -1, 1, 1);
        }
    }

    void Render(float r, float g, float b)
    {
        glBegin(GL_POINTS);
        for (uint i = 0; i < SIZE; i++)
        {
            auto &p = buffer.at(i);
            glColor3f(r, g, b);
            glVertex2f(p.p.x, p.p.y);
        }
        glEnd();
    }
};

#endif