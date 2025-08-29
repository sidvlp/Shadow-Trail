#pragma once
#include "../../src/Camera.h"
#include "../../src/BaseShader.h"

class ParticleShader : public BaseShader {
public:
    ParticleShader();
    void activate(const BaseCamera& Cam) const override;
};
