#version 330 core

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

out vec3 fColor;

float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
fColor = uKd * (uDirectionalLightIntensity * max(0.0, dot(vViewSpaceNormal, uDirectionalLightDir)) + uPointLightIntensity * max(0.0, dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight))



void main() {
    fColor = vViewSpaceNormal;
}
