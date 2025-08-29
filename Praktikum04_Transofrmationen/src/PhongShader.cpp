//
//  PhongShader.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "PhongShader.h"
#include "../CGVStudio/CGVStudio/ShaderLightMapper.h"



const char* VertexShaderCode =
"#version 400\n"
"layout(location=0) in vec4 VertexPos;"
"layout(location=1) in vec4 VertexNormal;"
"layout(location=2) in vec2 VertexTexcoord;"
"out vec3 Position;"
"out vec3 Normal;"
"out vec2 Texcoord;"
"uniform mat4 ModelMat;"
"uniform mat4 ModelViewProjMat;"
"void main()"
"{"
"    Position = (ModelMat * VertexPos).xyz;"
"    Normal =  (ModelMat * vec4(VertexNormal.xyz,0)).xyz;"
"    Texcoord = VertexTexcoord;"
"    gl_Position = ModelViewProjMat * VertexPos;"
"}";



const char* FragmentShaderCode = R"(
#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

out vec4 FragColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;
uniform float time;         
uniform bool isDarkPath;     


const int MAX_LIGHTS = 14;



struct ShaderLight {
  int Type;
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	vec3 Attenuation;
	vec3 SpotRadius;
	int ShadowIndex;
};

layout(std140) uniform Lights {
    int LightCount;
    ShaderLight lights[MAX_LIGHTS];
};

float sat(float a) { return clamp(a, 0.0, 1.0); }


vec3 calcBasicLighting(vec3 N, vec3 E, vec4 texColor, vec3 DiffuseColor, vec3 SpecularColor, float SpecularExp) {
    vec3 result = AmbientColor * texColor.rgb;

    for(int i = 0; i < LightCount; i++) {
        if(lights[i].Type != 0) continue;

        vec3 L = lights[i].Position - Position;
        float dist = length(L);
        L = normalize(L);

        float att = 1.0 / (
            lights[i].Attenuation.x +
            lights[i].Attenuation.y * dist +
            lights[i].Attenuation.z * dist * dist
        );

        float diff = sat(dot(N, L));
        vec3 R = reflect(-L, N);
        float spec = pow(sat(dot(R, E)), SpecularExp);

        vec3 diffuse = lights[i].Color * DiffuseColor * diff * att;
        vec3 specular = lights[i].Color * SpecularColor * spec * att;

        result += (diffuse + specular) * texColor.rgb;
    }

    return result;
}



float computeRingRadius(float time, out bool fullBright, out bool pulsing) {
    float growPhase = 2.0;
    float brightPhase = 0.5;
    float shrinkPhase = 2.0;
    float darkPulsePhase = 4.0;
    float cycleLength = growPhase + brightPhase + shrinkPhase + darkPulsePhase;

    float tCycle = mod(time, cycleLength);

    float pulse = sin(time * 3.1415);
    float pulsingRadius = 1.25 + 0.25 * pulse;

    fullBright = false;
    pulsing = false;

    if(tCycle < growPhase) {
        float t = tCycle / growPhase;
        return mix(pulsingRadius, 10.0, t);
    } else if(tCycle < growPhase + brightPhase) {
        fullBright = true;
        return 10.0;
    } else if(tCycle < growPhase + brightPhase + shrinkPhase) {
        float t = (tCycle - growPhase - brightPhase) / shrinkPhase;
        return mix(10.0, 1.0, t);
    } else {
        pulsing = true;
        return pulsingRadius;
    }
}

vec3 applyLight(ShaderLight light, vec3 N, vec3 E, vec4 texColor, float SpecularExp, float dist) {
    vec3 L = light.Position - Position;
    L = normalize(L);

    float att = 1.0 / (light.Attenuation.x + light.Attenuation.y*dist + light.Attenuation.z*dist*dist);
    float diff = sat(dot(N,L));
    vec3 R = reflect(-L, N);
    float spec = pow(sat(dot(R,E)), SpecularExp);

    vec3 diffuse  = light.Color * DiffuseColor * diff * att;
    vec3 specular = light.Color * SpecularColor * spec * att;

    return (diffuse + specular) * texColor.rgb;
}

bool isInsideRing(float dist, float ringRadius, float ringThickness) {
    float ringStart = ringRadius - ringThickness;
    return dist >= ringStart && dist <= ringRadius;
}

bool isBlocked(int currentLightIndex, float ringRadius) {
    for(int j=0; j<LightCount; j++) {
        if(j == currentLightIndex || lights[j].Type != 0) continue;
        float distToOther = length(lights[j].Position - lights[currentLightIndex].Position);
        if(distToOther < ringRadius * 2.0) {
            return true;
        }
    }
    return false;
}

vec3 calcAnimatedLighting(vec3 N, vec3 E, vec4 texColor, vec3 DiffuseColor, vec3 SpecularColor, float SpecularExp, float time, out bool insideAnyLightRadius) {
    vec3 result = AmbientColor * texColor.rgb;
    insideAnyLightRadius = false;

    bool fullBright, pulsing;
    float ringRadius = computeRingRadius(time, fullBright, pulsing);
    float ringThickness = 0.05;

    for(int i=0; i<LightCount; i++) {
        if(lights[i].Type != 0) continue;

        float dist = length(lights[i].Position - Position);
        result += applyLight(lights[i], N, E, texColor, SpecularExp, dist);

        if(dist <= ringRadius)
            insideAnyLightRadius = true;

        if(!isBlocked(i, ringRadius) && 
           isInsideRing(dist, ringRadius, ringThickness) && 
           (pulsing || !fullBright)) 
        {
            result += vec3(1.0, 0.3, 0.0);
        }
    }

    if(!fullBright && !insideAnyLightRadius) {
        result *= 0.0;
    }

    return result;
}



void main() {
    vec3 N = normalize(Normal);
    vec3 E = normalize(EyePos - Position);
    vec4 texColor = texture(DiffuseTexture, Texcoord);
    if(texColor.a < 0.3) discard;

    vec3 result;
    bool insideAnyLightRadius;

    if(!isDarkPath) {
        // Einfache Lichtberechnung ohne Effekt
        result = calcBasicLighting(N, E, texColor, DiffuseColor, SpecularColor, SpecularExp);
    } else {
        // Effekt aktiviert
        result = calcAnimatedLighting(N, E, texColor, DiffuseColor, SpecularColor, SpecularExp, time, insideAnyLightRadius);
    }

    FragColor = vec4(result, texColor.a);
}
)";



PhongShader::PhongShader() :
    DiffuseColor(0.8f, 0.8f, 0.8f),
    SpecularColor(0.5f, 0.5f, 0.5f),
    AmbientColor(0.2f, 0.2f, 0.2f),
    SpecularExp(20.0f),
    LightPos(20.0f, 20.0f, 20.0f),
    LightColor(1, 1, 1),
    DiffuseTexture(Texture::defaultTex()),
    UpdateState(0xFFFFFFFF)
{
    ShaderProgram = createShaderProgram(VertexShaderCode, FragmentShaderCode);
    assignLocations();
}
void PhongShader::assignLocations()
{
    DiffuseColorLoc = glGetUniformLocation(ShaderProgram, "DiffuseColor");
    AmbientColorLoc = glGetUniformLocation(ShaderProgram, "AmbientColor");
    SpecularColorLoc = glGetUniformLocation(ShaderProgram, "SpecularColor");
    SpecularExpLoc = glGetUniformLocation(ShaderProgram, "SpecularExp");
    DiffuseTexLoc = glGetUniformLocation(ShaderProgram, "DiffuseTexture");
    LightPosLoc = glGetUniformLocation(ShaderProgram, "LightPos");
    LightColorLoc = glGetUniformLocation(ShaderProgram, "LightColor");
    EyePosLoc = glGetUniformLocation(ShaderProgram, "EyePos");
    ModelMatLoc = glGetUniformLocation(ShaderProgram, "ModelMat");
    ModelViewProjLoc = glGetUniformLocation(ShaderProgram, "ModelViewProjMat");
    TimeLoc = glGetUniformLocation(ShaderProgram, "time");
    DarkPathLoc = glGetUniformLocation(ShaderProgram, "isDarkPath");
}
void PhongShader::activate(const BaseCamera& Cam) const
{
    BaseShader::activate(Cam);

    // update uniforms if necessary
    if (UpdateState & DIFF_COLOR_CHANGED)
        glUniform3f(DiffuseColorLoc, DiffuseColor.R, DiffuseColor.G, DiffuseColor.B);
    if (UpdateState & AMB_COLOR_CHANGED)
        glUniform3f(AmbientColorLoc, AmbientColor.R, AmbientColor.G, AmbientColor.B);
    if (UpdateState & SPEC_COLOR_CHANGED)
        glUniform3f(SpecularColorLoc, SpecularColor.R, SpecularColor.G, SpecularColor.B);
    if (UpdateState & SPEC_EXP_CHANGED)
        glUniform1f(SpecularExpLoc, SpecularExp);

    DiffuseTexture->activate(0);
    if (UpdateState & DIFF_TEX_CHANGED && DiffuseTexture)
        glUniform1i(DiffuseTexLoc, 0);

    if (UpdateState & LIGHT_COLOR_CHANGED)
        glUniform3f(LightColorLoc, LightColor.R, LightColor.G, LightColor.B);
    if (UpdateState & LIGHT_POS_CHANGED)
        glUniform3f(LightPosLoc, LightPos.X, LightPos.Y, LightPos.Z);

    // always update matrices
    Matrix ModelViewProj = Cam.getProjectionMatrix() * Cam.getViewMatrix() * modelTransform();
    glUniformMatrix4fv(ModelMatLoc, 1, GL_FALSE, modelTransform().m);
    glUniformMatrix4fv(ModelViewProjLoc, 1, GL_FALSE, ModelViewProj.m);

    Vector EyePos = Cam.position();
    glUniform3f(EyePosLoc, EyePos.X, EyePos.Y, EyePos.Z);

    glUniform1f(TimeLoc, glfwGetTime());
    glUniform1i(DarkPathLoc, isDarkPath ? 1 : 0);




    UpdateState = 0x0;

    GLuint UBO = ShaderLightMapper::instance().uniformBlockID();
    GLuint BlockIndex = glGetUniformBlockIndex(ShaderProgram, "Lights");
    glUniformBlockBinding(ShaderProgram, BlockIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);

}
void PhongShader::diffuseColor(const Color& c)
{
    DiffuseColor = c;
    UpdateState |= DIFF_COLOR_CHANGED;
}
void PhongShader::ambientColor(const Color& c)
{
    AmbientColor = c;
    UpdateState |= AMB_COLOR_CHANGED;
}
void PhongShader::specularColor(const Color& c)
{
    SpecularColor = c;
    UpdateState |= SPEC_COLOR_CHANGED;
}
void PhongShader::specularExp(float exp)
{
    SpecularExp = exp;
    UpdateState |= SPEC_EXP_CHANGED;
}
void PhongShader::lightPos(const Vector& pos)
{
    LightPos = pos;
    UpdateState |= LIGHT_POS_CHANGED;
}
void PhongShader::lightColor(const Color& c)
{
    LightColor = c;
    UpdateState |= LIGHT_COLOR_CHANGED;
}

void PhongShader::diffuseTexture(const Texture* pTex)
{
    DiffuseTexture = pTex;
    if (!DiffuseTexture)
        DiffuseTexture = Texture::defaultTex();

    UpdateState |= DIFF_TEX_CHANGED;
}

void PhongShader::setDarkPath(bool dark) {
    isDarkPath = dark;
}






