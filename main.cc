#include <iostream>
#include <array>
#include <omp.h>
#include <glm/glm.hpp>

#include "app.hh"
#include "particle.hh"
#include "shader.hh"
#include "util.hh"

enum class ShaderID
{
    ID,
    PARTICLE_BASIC,
    COUNT
};
const uint ShaderIDCount = static_cast<uint>(ShaderID::COUNT);

#define S1 1000
#define BUFFER_COUNT 1

class ParticleApplication : public Application
{
private:
    Library<ShaderIDCount, ShaderID> shaders;
    ParticleGroup<S1, BUFFER_COUNT> *b1;
public:
    void Setup() override
    {
        width = height = 750;
        ups = 30;
        fps = 25;
    }
    void Init() override
    {
        shaders.Load(ShaderID::PARTICLE_BASIC, "shaders/vpartbasic.glsl", "shaders/fpartbasic.glsl");
        glPointSize(2);
        b1 = new ParticleGroup<S1, BUFFER_COUNT>();
    }
    void Input() override
    {
    }
    void Update(float delta) override
    {
        b1->Update<S1>(*b1, .002, .005, .15, delta,
                       glm::vec4(-1, 1, -1, 1), 0.001);
    }
    void Render(float delta) override
    {
        b1->Render();
    }
};

int main(void)
{
    auto v1 = glm::vec4(1);
    ParticleApplication app;
    app.Start();
    return 0;
}