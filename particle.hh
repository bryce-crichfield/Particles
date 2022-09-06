#ifndef PARTICLE_HH
#define PARTICLE_HH

#include <cstdlib>
#include <iostream>
#include <array>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>

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

enum class ParticleAttribute
{
    POSITION = 0,
    VELOCITY = 1,
    COUNT = 2
};

const uint NUM_PARTICLE_ATTRIBUTES = static_cast<uint>(ParticleAttribute::COUNT);

template <uint SIZE, uint BUFFER_COUNT>
struct ParticleGroup
{
    uint current_buffer = 0;
    GLuint vao[BUFFER_COUNT], vbo[NUM_PARTICLE_ATTRIBUTES];

    std::array<std::array<glm::vec2, SIZE>,
               NUM_PARTICLE_ATTRIBUTES>
        particles;

    ParticleGroup()
    {
        for (uint attribute = 0; attribute < NUM_PARTICLE_ATTRIBUTES; attribute++) 
        {
            std::array<glm::vec2, SIZE>& pos = particles.at(static_cast<uint>(ParticleAttribute::POSITION));
            std::array<glm::vec2, SIZE>& vel = particles.at(static_cast<uint>(ParticleAttribute::VELOCITY));
            for (uint i = 0; i < SIZE; i++) 
            {
                pos.at(i) = glm::vec2(
                    random_between(-1, 1),
                    random_between(-1, 1));
                vel.at(i) = glm::vec2(
                    random_between(-.1, .1),
                    random_between(-.1, .1));
            }
        }


        auto attribute_byte_size = SIZE * sizeof(glm::vec2);
        glGenVertexArrays(BUFFER_COUNT, vao);
        glGenBuffers(NUM_PARTICLE_ATTRIBUTES, vbo);
        for(uint buffer = 0; buffer < BUFFER_COUNT; buffer++)
        {
            glBindVertexArray(vao[0]);
            // Bind First Attribute (Position)
            glBindBuffer(GL_ARRAY_BUFFER, 
                vbo[static_cast<uint>(ParticleAttribute::POSITION)]);
            glBufferData(GL_ARRAY_BUFFER,
                         attribute_byte_size,
                         particles.at(static_cast<uint>(ParticleAttribute::POSITION)).data(),
                         GL_DYNAMIC_DRAW);
            glVertexAttribPointer(static_cast<GLuint>(ParticleAttribute::POSITION),
                                  glm::vec2::length(), GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(static_cast<GLuint>(ParticleAttribute::POSITION));
            // Bind Second Attribute (Velocity)
            glBindBuffer(GL_ARRAY_BUFFER, 
                vbo[static_cast<uint>(ParticleAttribute::VELOCITY)]);
            glBufferData(GL_ARRAY_BUFFER,
                         attribute_byte_size,
                         particles.at(static_cast<uint>(ParticleAttribute::VELOCITY)).data(),
                         GL_DYNAMIC_DRAW);
            glVertexAttribPointer(static_cast<GLuint>(ParticleAttribute::VELOCITY),
                                  glm::vec2::length(), GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(static_cast<GLuint>(ParticleAttribute::VELOCITY));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }

    void CopyToGpu()
    {
        // We don't need to change VAO yet, just take the current one and dump it
        // Bind to Current Buffer
        glBindBuffer(GL_ARRAY_BUFFER, 
            vbo[static_cast<uint>(ParticleAttribute::POSITION)]);
        // Clear out the Buffer
        glBufferData(GL_ARRAY_BUFFER, 
            sizeof(glm::vec2)*SIZE,
            0, GL_DYNAMIC_DRAW);
        // Map the Buffer
        glm::vec2* m1 = reinterpret_cast<glm::vec2*> (
            glMapBufferRange(GL_ARRAY_BUFFER, 0,
            sizeof(glm::vec2)*particles[static_cast<uint>(ParticleAttribute::POSITION)].size(),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)
        );
        std::copy(
            particles[static_cast<uint>(ParticleAttribute::POSITION)].begin(), 
            particles[static_cast<uint>(ParticleAttribute::POSITION)].end(), m1);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        // Bind to Current Buffer
        glBindBuffer(GL_ARRAY_BUFFER, 
            vbo[static_cast<uint>(ParticleAttribute::VELOCITY)]);
        // Clear out the Buffer
        glBufferData(GL_ARRAY_BUFFER, 
            sizeof(glm::vec2)*SIZE,
            0, GL_DYNAMIC_DRAW);
        // Map the Buffer
        glm::vec2* m2 = reinterpret_cast<glm::vec2*> (
            glMapBufferRange(GL_ARRAY_BUFFER, 0,
            sizeof(glm::vec2)*particles[static_cast<uint>(ParticleAttribute::VELOCITY)].size(),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)
        );
        std::copy(
            particles[static_cast<uint>(ParticleAttribute::VELOCITY)].begin(), 
            particles[static_cast<uint>(ParticleAttribute::VELOCITY)].end(), m2);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    ~ParticleGroup()
    {
        glDisableVertexAttribArray(static_cast<GLuint>(ParticleAttribute::POSITION));
        glDisableVertexAttribArray(static_cast<GLuint>(ParticleAttribute::VELOCITY));
        glDeleteBuffers(NUM_PARTICLE_ATTRIBUTES, vbo);
        glDeleteVertexArrays(BUFFER_COUNT, vao);
    }


    template <uint S2>
    void Update(ParticleGroup<S2, BUFFER_COUNT> &that, float G, 
        float Min_R, float Max_R, float delta,
        glm::vec4 bounds, float drag)
    {
        #pragma omp parallel for
        for (uint i = 0; i < SIZE; i++)
        {
            glm::vec2& p1_p = particles.at(static_cast<uint>(ParticleAttribute::POSITION)).at(i);
            glm::vec2& p1_v = particles.at(static_cast<uint>(ParticleAttribute::VELOCITY)).at(i);

            glm::vec2 acceleration(0);
            for (uint j = 0; j < S2; j++) 
            {
                if (i == j) continue;
                glm::vec2& p2_p = that.particles.at(static_cast<uint>(ParticleAttribute::POSITION)).at(j);
                glm::vec2& p2_v = that.particles.at(static_cast<uint>(ParticleAttribute::VELOCITY)).at(j);
                auto difference = p1_p - p2_p;
                auto radius = glm::length(difference);
                if (radius != 0 && radius > Min_R && radius < Max_R)
                    acceleration += (difference / radius);
                if (radius < Min_R) {
                    acceleration -= difference / radius;
                }
            }
            p1_v += acceleration * -G * delta;

            // Apply the force of drag as a function of the current velocity
            // The greater the velocity, the greater the drag 
            if (abs(p1_v.x) < drag) p1_v.x = 0;
            else if (abs(p1_v.x) > 0.0f) {
                if (p1_v.x > 0.0f) p1_v.x -= p1_v.x * drag;
                if (p1_v.x < 0.0f) p1_v.x += (-p1_v.x) * drag;
            }
            if(abs(p1_v.y) < drag) p1_v.y = 0;
            if (abs(p1_v.y) > 0.0f) {
                if (p1_v.y > 0.0f) p1_v.y -= p1_v.y * drag;
                if (p1_v.y < 0.0f) p1_v.y += (-p1_v.y) * drag;
            }

            p1_p += p1_v * delta;
            limit(p1_p, bounds, 0);
            if (p1_p.x == bounds.x || p1_p.x == bounds.y)
                p1_v.x *= -1;   
            if (p1_p.y == bounds.z || p1_p.y == bounds.w)
                p1_v.y *= -1;
        }
        CopyToGpu();
    }

    void Render()
    {
        glBindVertexArray(vao[current_buffer]);
        glDrawArrays(GL_POINTS, 0, SIZE);
        current_buffer = (current_buffer + 1) % BUFFER_COUNT;
    }
};
#endif