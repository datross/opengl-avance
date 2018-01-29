#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

out vec3 vViewSpacePosition;
out vec3 vViewSpaceNormal;
out vec3 vTexCoords;

void main() {
    vec4 position = vec4(aPosition, 1);
    vec4 normal = vec4(aNormal, 0);
    
    vViewSpacePosition = vec3(uModelViewMatrix * position);
    vViewSpaceNormal = vec3(uNormalMatrix * normal);
    vTexCoords = aTexCoords;
    gl_Position = uModelViewProjMatrix * position;
}
