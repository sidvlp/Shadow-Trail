#include "ParticleShader.h"

// Minimaler Vertex-Shader: nimmt die Vertex-Position und multipliziert sie mit der MVP-Matrix
const char* VertexShader = R"(
#version 400
layout(location = 0) in vec3 VertexPos;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(VertexPos, 1.0);
    gl_PointSize = 5.0; // default Punktgröße
}
)";

// Minimaler Fragment-Shader: malt weiße Punkte
const char* FragmentShader = R"(
#version 400
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.0, 1.0);
}
)";

ParticleShader::ParticleShader() {
    ShaderProgram = createShaderProgram(VertexShader, FragmentShader);
}

void ParticleShader::activate(const BaseCamera& Cam) const {
    BaseShader::activate(Cam);

    Matrix MVP = Cam.getProjectionMatrix() * Cam.getViewMatrix() * modelTransform();
    GLint loc = glGetUniformLocation(ShaderProgram, "MVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, MVP.m);
}
