#version 450

layout(location = 0) out vec2 outUV;

vec2 positions[6] = vec2[](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, -1.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, -1.0f),
    vec2(0.0f, -1.0f)
);

vec2 uvs[6] = vec2[](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, 1.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f)
);

void main() {
   gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
   outUV = uvs[gl_VertexIndex];
}