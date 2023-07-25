#version 450
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform sampler2D quadTexture;

void main()
{
    FragColor = texture(quadTexture, uv);
}


