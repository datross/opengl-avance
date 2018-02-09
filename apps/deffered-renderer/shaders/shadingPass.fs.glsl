#version 330 core

uniform sampler2D uGPosition;
uniform sampler2D uGNormal;
uniform sampler2D uGAmbient;
uniform sampler2D uGDiffuse;
uniform sampler2D uGlossyShininess;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

out vec3 fColor;

void main() {
    vec3 position = vec3(texelFetch(uGPosition, ivec2(gl_FragCoord.xy), 0));
    vec3 normal   = vec3(texelFetch(uGNormal  , ivec2(gl_FragCoord.xy), 0));
    vec3 ambient  = vec3(texelFetch(uGAmbient , ivec2(gl_FragCoord.xy), 0));
    vec3 diffuse  = vec3(texelFetch(uGDiffuse , ivec2(gl_FragCoord.xy), 0));
    vec3 glossy   = vec3(texelFetch(uGlossyShininess, ivec2(gl_FragCoord.xy), 0));
    float shininess = float(texelFetch(uGlossyShininess, ivec2(gl_FragCoord.xy), 0).w);


    vec3 halfVector = 0.5 * (uDirectionalLightDir + position);
    fColor = uDirectionalLightIntensity * (
             diffuse * max(0.0, dot(normal, uDirectionalLightDir))
             + glossy * pow(max(0, dot(halfVector, normal)), shininess));
}
