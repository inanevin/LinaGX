#version 450
#include "test.glsl"

// Input and output variables
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in ivec2 inTexCoord2;
layout(location = 0) out vec4 outColor;

void main() {
    // Use the UBO and texture sampler in the shader
    gl_Position = vec4(inPosition, 1.0);
    outColor = vec4(0,0,0,1);
}