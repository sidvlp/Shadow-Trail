#pragma once
#include <vector>
#include "../../src/vector.h"
#include "../../src/color.h"
#include "../../src/Matrix.h"
#include "ParticleShader.h"

struct Particle {
    Vector Position = Vector(0, 0, 0);
    Vector Velocity = Vector(0, 0, 0);
    Color Color;
    float LifeTime = 0.0f;
    float MaxLifeTime = 0.0f;
    float Scale = 1.0f;
};



class ParticleSystem {
public:

    ParticleSystem(int maxParticles, const Vector& emitterPos);
    void Update(float dt);
    void Render(const Matrix& viewProjMatrix);

    void SetEmitterPosition(const Vector& pos);

private:
    void RespawnParticle(int index);
    int FindUnusedParticle();

    std::vector<Particle> m_Particles;
    Vector m_EmitterPosition;
    int m_MaxParticles;
};
