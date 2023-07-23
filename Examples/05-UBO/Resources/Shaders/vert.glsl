#version 450
 
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;
layout(location = 0) out vec2 outUV;

layout(set = 1, binding = 0) uniform UBO {
float time;
} ubo[2];

void main() {
    gl_Position = vec4(inPosition, 1.0);
    outUV = vec2(uv.x + sin(ubo[0].time), uv.y + sin(ubo[1].time));
}