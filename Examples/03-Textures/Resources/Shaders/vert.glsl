#version 450
 
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;
layout(location = 0) out vec2 outUV;

layout(set = 0, binding = 0) uniform HEBELE
{
    vec4 abc;
} hebele;

void main() {
    gl_Position = vec4(inPosition, hebele.abc.x);
    outUV = uv;
}
