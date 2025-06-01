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

enum class ParticleSpawnMode {
    Default,
    Ring
};


class ParticleSystem {
public:

    ParticleSystem(int maxParticles, const Vector& emitterPos, ParticleSpawnMode mode = ParticleSpawnMode::Default);
    void Update(float dt);
    void Render(const Matrix& viewProjMatrix);

    void SetEmitterPosition(const Vector& pos);
    Vector GetEmitterPosition() const;


private:
    void RespawnParticle(int index);
    void RespawnDefaultParticle(int index);
    void RespawnRingParticle(int index);
    int FindUnusedParticle();

    std::vector<Particle> m_Particles;
    Vector m_EmitterPosition;
    int m_MaxParticles;
    ParticleSpawnMode m_SpawnMode;
};

