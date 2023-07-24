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

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/Math.hpp"

namespace LinaGX
{

    enum class GLTFTextureType
    {
        BaseColor,
        Normal,
        Occlusion,
        Emissive,
        MetallicRoughness,
    };

    enum class GLTFAnimationProperty
    {
        Position,
        Rotation,
        Scale,
        Weights
    };

    enum class GLTFInterpolation
    {
        Linear,
        Step,
        CubicSpline,
    };

    struct ModelMaterial;

    struct ModelMeshPrimitive
    {
        ModelMaterial* material = nullptr;

        LINAGX_VEC<LGXVector2>     texCoords;
        LINAGX_VEC<LGXVector3>     positions;
        LINAGX_VEC<LGXVector3>     normals;
        LINAGX_VEC<LGXVector4>     tangents;
        LINAGX_VEC<LGXVector4>     colors;
        LINAGX_VEC<unsigned char>  indices;
        LINAGX_VEC<LGXVector4ui16> joints;
        LINAGX_VEC<LGXVector4>     weights;
        uint32                     vertexCount = 0;
        IndexType                  indexType   = IndexType::Uint16;

        inline void Clear()
        {
            texCoords.clear();
            positions.clear();
            normals.clear();
            tangents.clear();
            colors.clear();
            indices.clear();
            joints.clear();
            weights.clear();
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
        LINAGX_STRING                       name      = "";
        LGXVector4                          baseColor = {1.0f, 1.0f, 1.0f, 1.0f};
        LGXVector3                          emissive  = {1.0f, 1.0f, 1.0f};
        LINAGX_MAP<GLTFTextureType, uint32> textureIndices;
        float                               metallicFactor  = 0.0f;
        float                               roughnessFactor = 0.0f;
        float                               alphaCutoff     = 0.0f;
        bool                                doubleSided     = false;
        bool                                isOpaque        = true;
    };

    struct ModelSkin;

    struct ModelNode
    {
    public:
        uint32                 index  = 0;
        ModelMesh*             mesh   = nullptr;
        ModelSkin*             skin   = nullptr;
        ModelNode*             parent = nullptr;
        LINAGX_STRING          name   = "";
        LINAGX_VEC<ModelNode*> children;

        LINAGX_VEC<float> localMatrix;
        LINAGX_VEC<float> inverseBindMatrix;
        LGXVector3        position = {};
        LGXVector3        scale    = {1.0f, 1.0f, 1.0f};
        LGXVector4        quatRot  = {0.0f, 0.0f, 0.0f, 1.0f};

        ~ModelNode()
        {
            children.clear();
        }
    };

    struct ModelSkin
    {
        ModelNode*             rootJoint = nullptr;
        LINAGX_VEC<ModelNode*> joints;

        ~ModelSkin()
        {
            joints.clear();
        }
    };

    struct ModelAnimationChannel
    {
        GLTFAnimationProperty targetProperty = GLTFAnimationProperty::Position;
        GLTFInterpolation     interpolation  = GLTFInterpolation::Linear;
        ModelNode*            targetNode     = nullptr;
        LINAGX_VEC<float>     keyframeTimes  = {};
        LINAGX_VEC<float>     values         = {};
    };

    struct ModelAnimation
    {
        LINAGX_STRING                     name = "";
        LINAGX_VEC<ModelAnimationChannel> channels;
        float                             duration = 0.0f;
    };

    struct ModelData
    {
        LINAGX_VEC<ModelNode*> rootNodes;

        ModelNode*      allNodes           = nullptr;
        uint32          allNodesCount      = 0;
        ModelMaterial*  allMaterials       = nullptr;
        uint32          allMaterialsCount  = 0;
        ModelTexture*   allTextures        = nullptr;
        uint32          allTexturesCount   = 0;
        ModelMesh*      allMeshes          = nullptr;
        uint32          allMeshesCount     = 0;
        ModelSkin*      allSkins           = nullptr;
        uint32          allSkinsCount      = 0;
        ModelAnimation* allAnimations      = nullptr;
        uint32          allAnimationsCount = 0;

        ~ModelData()
        {
            if (allNodes != nullptr)
                delete[] allNodes;

            if (allMaterials != nullptr)
                delete[] allMaterials;

            if (allMeshes != nullptr)
                delete[] allMeshes;

            if (allSkins != nullptr)
                delete[] allSkins;

            if (allAnimations != nullptr)
                delete[] allAnimations;

            if (allTextures != nullptr)
            {
                for (uint32 i = 0; i < allTexturesCount; i++)
                {
                    delete[] allTextures[i].buffer.pixels;
                }

                delete[] allTextures;
            }

            allNodes      = nullptr;
            allMaterials  = nullptr;
            allMeshes     = nullptr;
            allTextures   = nullptr;
            allSkins      = nullptr;
            allAnimations = nullptr;
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
    LINAGX_API bool LoadGLTFBinary(const char* path, ModelData& outData, bool loadTexturesIfPresent = true);

    /// <summary>
    ///
    /// </summary>
    /// <param name="path"></param>
    /// <param name="outData"></param>
    /// <returns></returns>
    LINAGX_API bool LoadGLTFASCII(const char* path, ModelData& outData, bool loadTexturesIfPresent = true);

} // namespace LinaGX

#endif
