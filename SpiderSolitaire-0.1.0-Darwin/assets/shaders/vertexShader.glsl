#version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        uniform vec2 offset;
        uniform vec2 uSize;
        uniform float uScale;
        uniform vec2 uAspect;

        out vec2 TexCoords;
        out vec2 localPos;

        void main() {
            localPos = aPos.xy * uSize * uScale;

            vec2 aspectPos = localPos;
            float aspect = uAspect.x / uAspect.y;
            aspectPos.x /= aspect;

            gl_Position = vec4(aspectPos + offset, aPos.z, 1.0);
            TexCoords = aTexCoord;
        }