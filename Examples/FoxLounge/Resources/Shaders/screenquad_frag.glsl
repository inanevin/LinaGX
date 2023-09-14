#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 0, binding = 1) uniform texture2D allTextures[];
layout (set = 0, binding = 2) uniform sampler allSamplers[];



void main()
{
    FragColor = texture(sampler2D(allTextures[5], allSamplers[0]), uv);
}


