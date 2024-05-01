#version 460
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

LGX_DEFINE_INDEXED_INDIRECT;

layout(set = 0, binding = 0) buffer IndirectCommandsData
{
    LGX_INDEXED_INDIRECT_CMD cmds[];
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


