#include "ParticelSystem.h"
#include <cstdlib> 
#include <GL/glew.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



static float RandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

ParticleSystem::ParticleSystem(int maxParticles, const Vector& emitterPos, ParticleSpawnMode mode)
    : m_MaxParticles(maxParticles), m_EmitterPosition(emitterPos), m_SpawnMode(mode)
{
    m_Particles.resize(maxParticles);
    for (int i = 0; i < maxParticles; ++i)
        RespawnParticle(i);
}


void ParticleSystem::SetEmitterPosition(const Vector& pos) {
    m_EmitterPosition = pos;
}

void ParticleSystem::Update(float dt) {
    // Anzahl neuer Partikel pro Sekunde
    const int emissionRate = 50;
    static float emissionAccumulator = 0.0f;

    emissionAccumulator += dt * emissionRate;
    int newParticles = static_cast<int>(emissionAccumulator);
    emissionAccumulator -= newParticles;

    // Neue Partikel erzeugen
    for (int i = 0; i < newParticles; ++i) {
        int index = FindUnusedParticle();
        if (index >= 0) {
            RespawnParticle(index);
        }
    }

    // Existierende Partikel aktualisieren
    for (int i = 0; i < m_MaxParticles; ++i) {
        Particle& p = m_Particles[i];
        if (p.LifeTime > 0.0f) {
            p.LifeTime -= dt;
            p.Position += p.Velocity * dt;

            float lifeRatio = p.LifeTime / p.MaxLifeTime;
            p.Color.R = 1.0f;
            p.Color.G = 0.5f * lifeRatio;
            p.Color.B = 0.0f;
            p.Scale = 0.5f + 0.5f * (1.0f - lifeRatio);
        }
    }
}

int ParticleSystem::FindUnusedParticle() {
    for (int i = 0; i < m_MaxParticles; ++i) {
        if (m_Particles[i].LifeTime <= 0.0f) {
            return i;
        }
    }
    return -1;
}

void ParticleSystem::RespawnParticle(int index) {
    if (m_SpawnMode == ParticleSpawnMode::Ring)
        RespawnRingParticle(index);
    else
        RespawnDefaultParticle(index);
}


void ParticleSystem::RespawnDefaultParticle(int index) {
    Particle& p = m_Particles[index];
    p.Position = m_EmitterPosition + Vector(RandomFloat(-0.1f, 0.1f), 0.0f, RandomFloat(-0.1f, 0.1f));
    p.Velocity = Vector(RandomFloat(-0.1f, 0.1f), RandomFloat(0.2f, 0.5f), RandomFloat(-0.1f, 0.1f));

    p.MaxLifeTime = RandomFloat(1.0f, 2.5f);
    p.LifeTime = p.MaxLifeTime;

    //p.Color = Color(1.0f, 0.6f, 0.2f); // Feuerfarbe
    p.Scale = 0.5f;
}

void ParticleSystem::RespawnRingParticle(int index) {
    Particle& p = m_Particles[index];

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
    //p.Color = Color(1.0f, 0.8f, 0.4f); 
    p.Scale = 0.5f;
}



void ParticleSystem::Render(const Matrix& viewProjMatrix) {
    glBegin(GL_POINTS);
    for (auto& p : m_Particles) {
        if (p.LifeTime <= 0.0f) continue;

        glColor4f(p.Color.R, p.Color.G, p.Color.B, 1.0f); 
        glVertex3f(p.Position.X, p.Position.Y, p.Position.Z);
    }
    glEnd();
}

Vector ParticleSystem::GetEmitterPosition() const {
    return m_EmitterPosition;
}


