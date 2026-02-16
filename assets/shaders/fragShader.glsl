#version 330 core

in vec2 localPos;
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D cardTexture;
uniform vec3 uColor;
uniform vec3 uTintColor;
uniform float uTintAmount;
uniform float radius;
uniform vec2 uSize;
uniform float uScale;
uniform float uAlpha;
uniform float uDimAmount;

void main() {
    vec2 halfSize = 0.5 * uSize * uScale;
    float r = radius * uScale;

    vec2 p = abs(localPos);
    vec2 q = p - halfSize + vec2(r);
    float dist = length(max(q, 0.0));

    vec4 texColor = texture(cardTexture, TexCoords);
    vec3 baseColor = texColor.rgb * uDimAmount;
    vec3 finalColor = mix(baseColor, uTintColor, uTintAmount);

    if (dist > r) {
        discard;
    }

    FragColor = vec4(finalColor, texColor.a * uAlpha);
}