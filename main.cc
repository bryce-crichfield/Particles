#include <iostream>
#include <array>
#include <omp.h>
#include <glm/glm.hpp>

#include "app.hh"
#include "particle.hh"
#include "shader.hh"
#include "util.hh"

// #define FRAME_TIME 1000 / 60
// #define PARTICLE_COUNT 100
// #define INTERACTION_RADIUS .25
// #define SPEED 0.00005

// std::array<Particle, PARTICLE_COUNT> particles;

// void init()
// {
//     glPointSize(10);
//     for (uint j = 0; j < PARTICLE_COUNT; j++)
//     {
//         particles.data()[j] = Particle::RandomParticle();
//         auto p = particles.data()[j];
//         auto x = p.x;
//         auto y = p.y;
//     }
// }

// void update(float force)
// {
//     // #pragma omp parallel for
//     for (uint i = 0; i < PARTICLE_COUNT; i++)
//     {
//         Particle &p = particles.data()[i];
//         float dx = 0;
//         float dy = 0;
//         for (uint j = 0; j < PARTICLE_COUNT; j++)
//         {
//             Particle &p2 = particles.data()[j];
//             float lx = p.x - p2.x;
//             float ly = p.y - p2.y;
//             float r = length(lx, ly);
//             if (r < INTERACTION_RADIUS && r > 0)
//             {
//                 dx += lx / r;
//                 dy += ly / r;
//             }
//         }
//         p.vx += (dx * force);
//         p.vy += (dy * force);
//         p.x += p.vx * SPEED;
//         p.y += p.vy * SPEED;
//         if (p.x < -.75)
//             p.vx += SPEED;
//         if (p.x > SPEED)
//             p.vx += -SPEED;
//         if (p.y < -SPEED)
//             p.vy += SPEED;
//         if (p.y > .75)
//             p.vy += -SPEED;
//         if (p.x < -1)
//             p.x = -1;
//         if (p.x > 1)
//             p.x = 1;
//         if (p.y < -1)
//             p.y = -1;
//         if (p.y > 1)
//             p.y = 1;
//     }
// }

// void render()
// {
//     glBegin(GL_POINTS);
//     for (uint j = 0; j < PARTICLE_COUNT; j++)
//     {
//         Particle p = particles.data()[j];
//         glColor3f(1.0f, 0.0f, 0.0f);
//         glVertex2f(p.x, p.y);
//     }
//     glEnd();
// }

enum class ShaderID {
    ID, COUNT
};
const uint ShaderIDCount = static_cast<uint>(ShaderID::COUNT);




class ParticleApplication : public Application 
{
private:
    Library<ShaderIDCount, ShaderID> library;
    ParticleBuffer<100, Particle> buffer;
public:
    void Setup()  override {
        width = height = 500;
        // Init the Particel Buffer
    }
    void Init()   override { 
        library.Load(ShaderID::ID, "shaders/vid.glsl", "shaders/fid.glsl");
    }
    void Input()  override { }
    void Update() override { 
        buffer.Update();
    }
    void Render() override { 
        buffer.Render(library.Get(ShaderID::ID));
    }
};



int main(void)
{
    auto v1 = glm::vec4(1);
    ParticleApplication app;
    app.Start();
    return 0;
}