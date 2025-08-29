#include <cstdlib>
#include <GL/glew.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "ParticelSystem.h"

static float RandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

ParticleSystem::ParticleSystem(int maxParticles, const Vector& emitterPos, ParticleSpawnMode mode)
    : m_MaxParticles(maxParticles), m_EmitterPosition(emitterPos), m_SpawnMode(mode)
{
    m_Particles.resize(maxParticles);
    for (auto& p : m_Particles)
        RespawnParticle(p);
}

void ParticleSystem::Update(float dt) {
    for (auto& p : m_Particles) {
        p.LifeTime -= dt;

        if (p.LifeTime <= 0.0f) {
            RespawnParticle(p);
        }
        else {
            p.Position += p.Velocity * dt;
        }
    }
}

void ParticleSystem::RespawnParticle(Particle& p) {
    if (m_SpawnMode == ParticleSpawnMode::Ring)
        RespawnRingParticle(p);
    else
        RespawnDefaultParticle(p);
}

void ParticleSystem::RespawnDefaultParticle(Particle& p) {
    p.Position = m_EmitterPosition + Vector(RandomFloat(-0.1f, 0.1f), 0.0f, RandomFloat(-0.1f, 0.1f));
    p.Velocity = Vector(RandomFloat(-0.1f, 0.1f), RandomFloat(0.2f, 0.5f), RandomFloat(-0.1f, 0.1f));
    p.MaxLifeTime = RandomFloat(1.0f, 2.5f);
    p.LifeTime = p.MaxLifeTime;
}

void ParticleSystem::RespawnRingParticle(Particle& p) {
    float radius = 0.7f;
    float angle = RandomFloat(0.0f, 2.0f * M_PI);

    float x = cos(angle) * radius + RandomFloat(-0.02f, 0.02f);
    float z = sin(angle) * radius + RandomFloat(-0.02f, 0.02f);

    p.Position = m_EmitterPosition + Vector(x, 0.0f, z);

    Vector outward = Vector(cos(angle), 0.0f, sin(angle)) * RandomFloat(0.02f, 0.05f);
    float upward = RandomFloat(0.01f, 0.05f);

    p.Velocity = outward + Vector(0.0f, upward, 0.0f);
    p.MaxLifeTime = RandomFloat(1.2f, 1.8f);
    p.LifeTime = p.MaxLifeTime;
}

void ParticleSystem::Render(const Matrix& viewProjMatrix) {
    glBegin(GL_POINTS);

    for (auto& p : m_Particles) {
        if (p.LifeTime > 0.0f) {
            glVertex3f(p.Position.X, p.Position.Y, p.Position.Z);
        }
    }
    glEnd();
}
