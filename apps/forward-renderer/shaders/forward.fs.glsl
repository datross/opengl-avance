#version 330 core

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

out vec3 fColor;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;;

uniform sampler2D uSamplerKa;
uniform sampler2D uSamplerKd;
uniform sampler2D uSamplerKs;
uniform sampler2D uSamplerShininess;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

void main() {
    vec3 halfVector = 0.5 * (uDirectionalLightDir + vViewSpacePosition);
    fColor = uDirectionalLightIntensity * (
             uKd * vec3(texture(uSamplerKd, vTexCoords)) * max(0.0, dot(vViewSpaceNormal, uDirectionalLightDir))
             + uKs * pow(max(0, dot(halfVector, vViewSpaceNormal)), uShininess));
}
