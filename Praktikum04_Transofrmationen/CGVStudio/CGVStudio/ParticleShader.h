#pragma once

#include "../../src/Matrix.h"
#include "../../src/color.h"
#include "../../src/baseshader.h"

class ParticleShader : public BaseShader {
public:
    ParticleShader();

    void activate(const BaseCamera& Cam) const override;

    void pointSize(float size);
    void particleColor(const Color& c);
    void setTime(float t);

private:
    void assignLocations();

    float PointSize;
    Color ParticleColor;
    float Time;

    GLint PointSizeLoc;
    GLint ParticleColorLoc;
    GLint TimeLoc;
};
