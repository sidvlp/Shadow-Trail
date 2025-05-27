#include "ParticleShader.h"

const char* ParticleVertexShader = R"(
#version 400
layout(location = 0) in vec3 VertexPos;

uniform mat4 ModelViewProjMat;
uniform float PointSize;

void main() {
    gl_Position = ModelViewProjMat * vec4(VertexPos, 1.0);
    gl_PointSize = PointSize;
}
)";

const char* ParticleFragmentShader = R"(
#version 400

out vec4 FragColor;
uniform vec3 ParticleColor;
uniform float time;

void main() {
    float alpha = 0.5 + 0.5 * sin(time * 3.14);
    FragColor = vec4(ParticleColor, alpha);
}
)";

ParticleShader::ParticleShader() :
    PointSize(10.0f), ParticleColor(1, 1, 1), Time(0.0f)
{
    ShaderProgram = createShaderProgram(ParticleVertexShader, ParticleFragmentShader);
    assignLocations();
}

void ParticleShader::assignLocations() {
    PointSizeLoc = glGetUniformLocation(ShaderProgram, "PointSize");
    ParticleColorLoc = glGetUniformLocation(ShaderProgram, "ParticleColor");
    TimeLoc = glGetUniformLocation(ShaderProgram, "time");
}

void ParticleShader::activate(const BaseCamera& Cam) const {
    BaseShader::activate(Cam);

    Matrix MVP = Cam.getProjectionMatrix() * Cam.getViewMatrix() * modelTransform();
    glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "ModelViewProjMat"), 1, GL_FALSE, MVP.m);

    glUniform1f(PointSizeLoc, PointSize);
    glUniform3f(ParticleColorLoc, ParticleColor.R, ParticleColor.G, ParticleColor.B);
    glUniform1f(TimeLoc, Time);
}

void ParticleShader::pointSize(float size) {
    PointSize = size;
}

void ParticleShader::particleColor(const Color& c) {
    ParticleColor = c;
}

void ParticleShader::setTime(float t) {
    Time = t;
}
