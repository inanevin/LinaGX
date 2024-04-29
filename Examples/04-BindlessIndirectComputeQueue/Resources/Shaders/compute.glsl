#version 460
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct IndirectCommand
{
    uint indexCount;
    uint instanceCount;
    uint firstIndex;
    int  vertexOffset;
    uint firstInstance;
};

layout(set = 0, binding = 0) buffer IndirectCommandsData
{
    IndirectCommand cmds[];
} indirectCommands;


void main()
{
     uint id = gl_GlobalInvocationID.z;

     // super dummy compute work
     // invalidate indirect draw id with 0
     if(id == 0)
     {
         indirectCommands.cmds[id].instanceCount = 0;
     }
}


