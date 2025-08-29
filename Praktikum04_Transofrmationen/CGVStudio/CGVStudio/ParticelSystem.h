#pragma once
#include <vector>
#include "../../src/Matrix.h"
#include "../../src/vector.h"

enum class ParticleSpawnMode {
    Default,
    Ring
};

struct Particle {
    Vector Position;
    Vector Velocity;
    float LifeTime;
    float MaxLifeTime;
};

class ParticleSystem {
public:
    ParticleSystem(int maxParticles, const Vector& emitterPos, ParticleSpawnMode mode);

    void Update(float dt);
    void Render(const Matrix& viewProjMatrix);

private:
    void RespawnParticle(Particle& p);
    void RespawnDefaultParticle(Particle& p);
    void RespawnRingParticle(Particle& p);

    int m_MaxParticles;
    Vector m_EmitterPosition;
    ParticleSpawnMode m_SpawnMode;
    std::vector<Particle> m_Particles;
};
