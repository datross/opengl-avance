#version 330 core

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

layout(location = 0) out vec3 fPosition;
layout(location = 1) out vec3 fNormal;
layout(location = 2) out vec3 fAmbient;
layout(location = 3) out vec3 fDiffuse;
layout(location = 4) out vec4 fGlossyShininess;

uniform sampler2D uSamplerKa;
uniform sampler2D uSamplerKd;
uniform sampler2D uSamplerKs;
uniform sampler2D uSamplerShininess;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

void main() {
    fPosition = vViewSpacePosition;
    fNormal = normalize(vViewSpaceNormal);
    fAmbient = uKa * vec3(texture(uSamplerKa, vTexCoords));
    fDiffuse = uKd * vec3(texture(uSamplerKd, vTexCoords));
    fGlossyShininess = vec4(uShininess * vec3(texture(uSamplerKs, vTexCoords)), uShininess * vec3(texture(uSamplerShininess, vTexCoords)));
}
