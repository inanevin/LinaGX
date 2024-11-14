/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2023-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

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
        LINAGX_VEC<LGXVector4ui16> jointsui16; // either jointsui8 or jointsui16 will be filled.
        LINAGX_VEC<LGXVector4ui8>  jointsui8;  // either jointsui8 or jointsui16 will be filled.
        LINAGX_VEC<LGXVector4>     weights;
        uint32                     vertexCount = 0;
        IndexType                  indexType   = IndexType::Uint16;
        LGXVector3                 minPosition = LGXVector3();
        LGXVector3                 maxPosition = LGXVector3();

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
        LINAGX_STRING                                    name      = "";
        uint32                                           index     = 0;
        LGXVector4                                       baseColor = {1.0f, 1.0f, 1.0f, 1.0f};
        LGXVector3                                       emissive  = {1.0f, 1.0f, 1.0f};
        LINAGX_VEC<LINAGX_PAIR<GLTFTextureType, uint32>> textureIndices;
        float                                            metallicFactor    = 0.0f;
        float                                            roughnessFactor   = 0.0f;
        float                                            alphaCutoff       = 0.0f;
        float                                            occlusionStrength = 0.0f;
        LGXVector3                                       emissiveFactor;
        bool                                             doubleSided = false;
        bool                                             isOpaque    = true;
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
        LINAGX_VEC<float>      localMatrix; // Either localMatrix will be non-empty, or position-scale-quatRot parameters will be filled.
        LINAGX_VEC<float>      inverseBindMatrix;
        LGXVector3             position = {};
        LGXVector3             scale    = {1.0f, 1.0f, 1.0f};
        LGXVector4             quatRot  = {0.0f, 0.0f, 0.0f, 1.0f};

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
        LINAGX_VEC<ModelNode*>      rootNodes; // All nodes without a parent.
        LINAGX_VEC<ModelNode*>      allNodes;  // All nodes regardless of whether they are children or not.
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
    /// Load a .glb file.
    /// </summary>
    LINAGX_API bool LoadGLTFBinary(const char* path, ModelData& outData);

    /// <summary>
    /// Load a .gltf file, if .bin file is present in the same directory it will be loaded and necessary data will be filled.
    /// </summary>
    LINAGX_API bool LoadGLTFASCII(const char* path, ModelData& outData);

} // namespace LinaGX
