#version 460
 
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 outUV;
layout(location = 1) flat out uint outDrawID;

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 viewProj;
} scene;

struct Object
{
    mat4 modelMatrix;
};

struct IndirectArguments
{
    int objectID;
    int materialByteIndex;
    int padding[2];
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout(set = 1, binding = 1) readonly buffer IndirectArgumentsData
{
    IndirectArguments args[];
} indirectArguments;

void main() {
    outDrawID = gl_DrawID;
    gl_Position = scene.viewProj * objectData.objects[indirectArguments.args[outDrawID].objectID].modelMatrix * vec4(inPosition, 1.0);
    outUV = vec2(uv.x, uv.y);
}
