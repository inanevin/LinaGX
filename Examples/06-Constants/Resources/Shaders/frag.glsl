#version 450
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform sampler2D texSampler;

layout( push_constant ) uniform constants
{
	vec4 tint;
} Constants;

void main()
{
    FragColor = texture(texSampler, uv) * Constants.tint;
}