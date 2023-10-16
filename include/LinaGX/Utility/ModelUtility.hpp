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

#include "LinaGX/Common/CommonGfx.hpp"
#include "LinaGX/Common/Math.hpp"

namespace LinaGX
{

    enum class GLTFTextureType
    {
        BaseColor,
        Normal,
        MetallicRoughness,
        Occlusion,
        Emissive,
        Max
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
        LINAGX_VEC<LGXVector4ui16> jointsui16;
        LINAGX_VEC<LGXVector4ui8>  jointsui8;
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
            jointsui16.clear();
            jointsui8.clear();
            weights.clear();
        }
    };

    struct ModelNode;

    struct ModelMesh
    {
        LINAGX_VEC<ModelMeshPrimitive*> primitives;
        ModelNode*                      node = nullptr;
        LINAGX_STRING                   name = "";

        ~ModelMesh()
        {
            if (!primitives.empty())
            {
                for (auto prim : primitives)
                {
                    prim->Clear();
                }

                delete[] primitives[0];
            }

            primitives.clear();
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
        uint32                              index     = 0;
        LGXVector4                          baseColor = {1.0f, 1.0f, 1.0f, 1.0f};
        LGXVector3                          emissive  = {1.0f, 1.0f, 1.0f};
        LINAGX_MAP<GLTFTextureType, uint32> textureIndices;
        float                               metallicFactor    = 0.0f;
        float                               roughnessFactor   = 0.0f;
        float                               alphaCutoff       = 0.0f;
        float                               occlusionStrength = 0.0f;
        LGXVector3                          emissiveFactor;
        bool                                doubleSided = false;
        bool                                isOpaque    = true;
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
        LINAGX_VEC<ModelNode*>      rootNodes;
        LINAGX_VEC<ModelNode*>      allNodes;
        LINAGX_VEC<ModelMaterial*>  allMaterials;
        LINAGX_VEC<ModelTexture*>   allTextures;
        LINAGX_VEC<ModelMesh*>      allMeshes;
        LINAGX_VEC<ModelSkin*>      allSkins;
        LINAGX_VEC<ModelAnimation*> allAnims;

        ~ModelData()
        {
            if (!allNodes.empty())
                delete[] allNodes[0];

            if (!allMaterials.empty())
                delete[] allMaterials[0];

            if (!allMeshes.empty())
                delete[] allMeshes[0];

            if (!allSkins.empty())
                delete[] allSkins[0];

            if (!allAnims.empty())
                delete[] allAnims[0];

            if (!allTextures.empty())
            {
                for (auto& b : allTextures)
                {
                    delete[] b->buffer.pixels;
                }

                delete[] allTextures[0];
            }

            allNodes.clear();
            allMaterials.clear();
            allMeshes.clear();
            allTextures.clear();
            allSkins.clear();
            allAnims.clear();
            rootNodes.clear();
        }
    };

    /// <summary>
    /// Preserve!
    /// </summary>
    /// <param name="path"></param>
    /// <param name="outData"></param>
    /// <param name="channelMask"></param>
    /// <returns></returns>
    LINAGX_API bool LoadGLTFBinary(const char* path, ModelData& outData);

    /// <summary>
    /// Preserve!
    /// </summary>
    /// <param name="path"></param>
    /// <param name="outData"></param>
    /// <returns></returns>
    LINAGX_API bool LoadGLTFASCII(const char* path, ModelData& outData);

} // namespace LinaGX
