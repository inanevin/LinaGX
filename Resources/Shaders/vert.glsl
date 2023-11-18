#version 450
layout(location = 0) out vec4 fragColor;
 
vec2 positions[3] = vec2[](
    vec2(0.0f, 0.5f),
    vec2(0.5f, -0.5f),
    vec2(-0.5f, -0.5f)
);
 
vec3 colors[3] = vec3[](
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f)
);
 
out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = vec4(colors[gl_VertexIndex], 1.0f);
}