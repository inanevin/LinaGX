/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2022-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifndef LINAGX_MODEL_UTILITY_HPP
#define LINAGX_MODEL_UTILITY_HPP

#include "Common/Common.hpp"

namespace LinaGX
{

#define LINAGX_TEXTURE_BASECOLOR          "BaseColor"
#define LINAGX_TEXTURE_NORMAL             "Normal"
#define LINAGX_TEXTURE_OCCLUSION          "Occlusion"
#define LINAGX_TEXTURE_EMISSIVE           "Emissive"
#define LINAGX_TEXTURE_METALLIC_ROUGHNESS "MetallicRoughness"

    struct ModelMeshPrimitive
    {
        LINAGX_VEC<LINAGX_VECTOR2> texCoords;
        LINAGX_VEC<LINAGX_VECTOR3> vertices;
        LINAGX_VEC<LINAGX_VECTOR3> normals;
        LINAGX_VEC<LINAGX_VECTOR4> tangents;
        LINAGX_VEC<LINAGX_VECTOR4> colors;
        LINAGX_VEC<uint32>         indices;
        uint32                     vertexCount = 0;

        inline void Clear()
        {
            texCoords.clear();
            vertices.clear();
            normals.clear();
            tangents.clear();
            colors.clear();
            indices.clear();
        }
    };

    struct ModelNode;

    struct ModelMesh
    {
        ModelMeshPrimitive* primitives     = nullptr;
        uint32              primitiveCount = 0;
        ModelNode*          node           = nullptr;
        LINAGX_STRING       name           = "";

        ~ModelMesh()
        {
            if (primitives != nullptr)
            {
                for (uint32 i = 0; i < primitiveCount; i++)
                {
                    ModelMeshPrimitive* prim = primitives + i;
                    prim->Clear();
                }

                delete[] primitives;
            }

            primitives = nullptr;
        }
    };

    struct ModelTexture
    {
        TextureBuffer buffer;
        LINAGX_STRING name = "";
    };

    struct ModelMaterial
    {
        LINAGX_STRING                     name      = "";
        LINAGX_VECTOR4                    baseColor = {1.0f, 1.0f, 1.0f, 1.0f};
        LINAGX_VECTOR3                    emissive  = {1.0f, 1.0f, 1.0f};
        LINAGX_MAP<LINAGX_STRING, uint32> textureIndices;
        float                             metallicFactor  = 0.0f;
        float                             roughnessFactor = 0.0f;
        float                             alphaCutoff     = 0.0f;
        bool                              doubleSided     = false;
        bool                              isOpaque        = true;
    };

    struct ModelNode
    {
    public:
        ModelMesh*             mesh   = nullptr;
        ModelNode*             parent = nullptr;
        LINAGX_VEC<ModelNode*> children;

        LINAGX_VECTOR3 position = {};
        LINAGX_VECTOR3 scale    = {1.0f, 1.0f, 1.0f};
        LINAGX_VECTOR4 quatRot  = {};
        LINAGX_MAT4    globalMatrix;
        LINAGX_MAT4    localMatrix;

        ~ModelNode()
        {
            children.clear();
        }
    };

    struct ModelData
    {
        ModelNode*             allNodes      = nullptr;
        uint32                 allNodesCount = 0;
        LINAGX_VEC<ModelNode*> rootNodes;

        ModelMaterial* allMaterials      = nullptr;
        uint32         allMaterialsCount = 0;

        ModelTexture* allTextures      = nullptr;
        uint32        allTexturesCount = 0;

        ModelMesh* allMeshes      = nullptr;
        uint32     allMeshesCount = 0;

        ~ModelData()
        {
            Clear();
        }

        void Clear()
        {
            if (allNodes != nullptr)
                delete[] allNodes;

            if (allMaterials != nullptr)
                delete[] allMaterials;

            if (allMeshes != nullptr)
                delete[] allMeshes;

            if (allTextures != nullptr)
            {
                for (uint32 i = 0; i < allTexturesCount; i++)
                {
                    delete[] allTextures[i].buffer.pixels;
                }

                delete[] allTextures;
            }

            allNodes     = nullptr;
            allMaterials = nullptr;
            allMeshes    = nullptr;
            allTextures  = nullptr;
            rootNodes.clear();
        }
    };

    /// <summary>
    ///
    /// </summary>
    /// <param name="path"></param>
    /// <param name="outData"></param>
    /// <param name="channelMask"></param>
    /// <returns></returns>
    LINAGX_API void LoadGLTFBinary(const char* path, ModelData& outData);

    /// <summary>
    ///
    /// </summary>
    /// <param name="path"></param>
    /// <param name="outData"></param>
    /// <returns></returns>
    LINAGX_API void LoadGLTFASCII(const char* path, ModelData& outData);

} // namespace LinaGX

#endif
