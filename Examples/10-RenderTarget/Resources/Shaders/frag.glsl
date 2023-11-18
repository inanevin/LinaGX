#version 450
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform sampler2D albedo;

layout( push_constant ) uniform constantsFrag
{
	layout(offset = 4) int test;
} ConstantsFrag;

void main()
{
    FragColor = texture(albedo, uv) * vec4(ConstantsFrag.test, ConstantsFrag.test, ConstantsFrag.test, 1);
}


