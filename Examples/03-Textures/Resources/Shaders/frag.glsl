#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 1, binding = 0) uniform sampler2D texSampler[];

layout(set = 0, binding = 0) uniform HEBELE
{
    vec4 abc;
    int indexu;
} hebele;

struct Test
{
  vec4 xd;
};

layout(set = 0, binding = 1) readonly buffer mySSBO
{
    Test test[];
}myTests;

void main()
{
    int index = hebele.indexu;
    vec2 realUV = uv + vec2(0, myTests.test[2].xd.x) + vec2(hebele.abc.r, 0);
    FragColor = texture(texSampler[index], realUV);
    // FragColor = vec4(uv.x, uv.y,1,1);
}
