#version 450
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform sampler2D albedo;

layout( push_constant ) uniform constants
{
	int objectID;
    int hasSkin;
    float alphaCutoff;
} Constants;


void main()
{
    vec4 texColor = texture(albedo, uv);

    if(texColor.a < Constants.alphaCutoff)
    {
        discard;
    }

    FragColor = texColor;
}


