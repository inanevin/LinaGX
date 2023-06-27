#version 450
layout(location = 0) out vec4 fragColor;
 
vec2 positions[3] = vec2[](
    vec2(0.0f, -0.5f),
    vec2(0.5f, 0.5f),
    vec2(-0.5f, 0.5f)
);
 
vec3 colors[3] = vec3[](
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f)
);
 
void main() {
    int idx = gl_VertexIndex;
 
    if (idx == 0) {
        gl_Position = vec4(positions[0], 0.0, 1.0);
    } else if (idx == 1) {
        gl_Position = vec4(positions[1], 0.0, 1.0);
    } else {
        gl_Position = vec4(positions[2], 0.0, 1.0);
    }
 
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}