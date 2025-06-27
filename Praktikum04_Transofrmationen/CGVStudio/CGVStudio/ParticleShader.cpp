#include "ParticleShader.h"
#include "../../src/Texture.h"
#include "../../src/Application.h"


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
/*

const char* ParticleFragmentShader = R"(
#version 400
uniform sampler2D ParticleTex;
uniform vec3 ParticleColor;
uniform float time;

out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord;
    vec4 texColor = texture(ParticleTex, coord);
    float flicker = 0.5 + 0.5 * sin(time * 6.2831);
    vec4 color = vec4(ParticleColor, 1.0) * texColor * flicker;
    
    if(color.a < 0.1) discard;
    FragColor = color;
}

)";
*/


ParticleShader::ParticleShader(const Texture* tex) :
    PointSize(10.0f), ParticleColor(1, 1, 1), Time(0.0f), FireTexture(tex)
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

   // FireTexture->activate(0);
    //glUniform1i(glGetUniformLocation(ShaderProgram, "ParticleTex"), 0);

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
