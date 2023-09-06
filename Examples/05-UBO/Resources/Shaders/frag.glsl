#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform UBO {
int time;
} ubo[2];


layout (set = 0, binding = 1) uniform sampler2D texSampler[];

void main()
{
    FragColor = texture(texSampler[ubo[0].time], uv);
}
