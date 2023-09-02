
#version 450
 
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;


layout(location = 0) out vec2 outUV;

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 viewProj;
} scene;

struct Object
{
    mat4 modelMatrix;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout( push_constant ) uniform constants
{
	int objectID;
    int materialByteIndex;
} Constants;

void main() {
    gl_Position = scene.viewProj * objectData.objects[Constants.objectID].modelMatrix * vec4(inPosition, 1.0);
    outUV = vec2(uv.x, uv.y);
}
