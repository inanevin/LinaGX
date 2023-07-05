#version 450
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform sampler2D texSampler;

struct TriangleProperties
{
    vec4 color;
    vec4 positionOffset;
};

layout(std140, set = 0, binding = 1) readonly buffer TriangleData
{
    TriangleProperties triangles[];
} triangleData;

layout( push_constant ) uniform constants
{
	int triangleIndex;
} Constants;


void main()
{
    FragColor = texture(texSampler, uv) * triangleData.triangles[Constants.triangleIndex].color;
}


