#pragma once

#include "../../src/Matrix.h"
#include "../../src/color.h"
#include "../../src/baseshader.h"
#include "../../src/Texture.h"

class ParticleShader : public BaseShader {
public:
    ParticleShader(const Texture* tex = nullptr);

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

    const Texture* FireTexture = nullptr;
    void testTexturePaths();
};
