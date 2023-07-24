
#version 450
 
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;
layout(location = 0) out vec2 outUV;

struct TriangleProperties
{
    vec4 color;
    vec4 positionOffset;
};

layout(std140, set = 1, binding = 0) readonly buffer TriangleData
{
    TriangleProperties triangles[];
} triangleData;

layout( push_constant ) uniform constants
{
	int triangleIndex;
} Constants;

void main() {
    gl_Position = vec4(inPosition.x + triangleData.triangles[Constants.triangleIndex].positionOffset.x, inPosition.y + triangleData.triangles[Constants.triangleIndex].positionOffset.y, inPosition.z, 1.0);
    outUV = vec2(uv.x, uv.y);
}