#version 450
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform sampler2DArray texSampler;

void main()
{
    FragColor = texture(texSampler, vec3(uv, 2));
    // FragColor = vec4(uv.x, uv.y,1,1);
}
