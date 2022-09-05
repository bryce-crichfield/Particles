#include <iostream>
#include <array>
#include <omp.h>
#include <glm/glm.hpp>

#include "app.hh"
#include "particle.hh"
#include "shader.hh"
#include "util.hh"

enum class ShaderID {
    ID, COUNT
};
const uint ShaderIDCount = static_cast<uint>(ShaderID::COUNT);

#define S1 15
#define S2 15

class ParticleApplication : public Application 
{
private:
    ParticleBuffer<S1> b1;
    ParticleBuffer<S2> b2; 
public:
    void Setup()  override {
        width = height = 2000;
        fps = 60;
    }
    void Init()   override { 
        glPointSize(10);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    void Input()  override { }
    void Update(float delta) override { 
        // Like Kind Attraction
        b1.Interact<S1>(b1, .5 , .05, .25, delta);
        b2.Interact<S2>(b2, .75, .05, .25, delta);
        // Unlike Kind Repulsion
        b1.Interact<S2>(b2, -.05, .25, .5, delta);
        b2.Interact<S1>(b1, -.25, .25, .5, delta);
    }
    void Render(float delta) override { 
        b1.Render(1.0f, 0.0f, 0.0f);
        b2.Render(0.0f, 1.0f, 0.0f);
    }
};

int main(void)
{
    auto v1 = glm::vec4(1);
    ParticleApplication app;
    app.Start();
    return 0;
}