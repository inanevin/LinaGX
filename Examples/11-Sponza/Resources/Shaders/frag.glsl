#version 450
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout(set = 2, binding = 0) uniform MaterialData
{
    vec4 baseColorTint;
    float alphaCutoff;
} material;

layout (set = 2, binding = 1) uniform sampler2D albedo;

void main()
{
    vec4 texColor = texture(albedo, uv) * material.baseColorTint;

    if(texColor.a < material.alphaCutoff)
    {
        discard;
    }

    FragColor = texColor;
}


