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

#include "Utility/ModelUtility.hpp"

#define TINYGLTF_IMPLEMENTATION
#include "Utility/tinygltf/tiny_gltf.h"

namespace LinaGX
{
    Matrix4 CalculateGlobalMatrix(ModelNode* node)
    {
        if (node->parent != nullptr)
            return node->localMatrix * CalculateGlobalMatrix(node->parent);

        return node->localMatrix;
    }

    void ProcessGLTF(tinygltf::Model& model, ModelData& outData)
    {
        outData.allNodes      = new ModelNode[model.nodes.size()];
        outData.allNodesCount = static_cast<uint32>(model.nodes.size());

        if (!model.materials.empty())
        {
            outData.allMaterials      = new ModelMaterial[model.materials.size()];
            outData.allMaterialsCount = static_cast<uint32>(model.materials.size());

            for (size_t i = 0; i < model.materials.size(); i++)
            {
                const auto&    gltfMat = model.materials[i];
                ModelMaterial* mat     = outData.allMaterials + i;
                mat->name              = gltfMat.name;
                mat->metallicFactor    = static_cast<float>(gltfMat.pbrMetallicRoughness.metallicFactor);
                mat->roughnessFactor   = static_cast<float>(gltfMat.pbrMetallicRoughness.roughnessFactor);
                mat->emissive          = {static_cast<float>(gltfMat.emissiveFactor[0]), static_cast<float>(gltfMat.emissiveFactor[1]), static_cast<float>(gltfMat.emissiveFactor[2])};
                mat->baseColor         = {static_cast<float>(gltfMat.pbrMetallicRoughness.baseColorFactor[0]), static_cast<float>(gltfMat.pbrMetallicRoughness.baseColorFactor[1]), static_cast<float>(gltfMat.pbrMetallicRoughness.baseColorFactor[2]), static_cast<float>(gltfMat.pbrMetallicRoughness.baseColorFactor[3])};
                mat->alphaCutoff       = static_cast<float>(gltfMat.alphaCutoff);
                mat->isOpaque          = gltfMat.alphaMode.compare("OPAQUE") == 0;
                mat->doubleSided       = gltfMat.doubleSided;

                if (gltfMat.emissiveTexture.index != -1)
                    mat->textureIndices[GLTFTextureType::Emissive] = gltfMat.emissiveTexture.index;

                if (gltfMat.normalTexture.index != -1)
                    mat->textureIndices[GLTFTextureType::Normal] = gltfMat.normalTexture.index;

                if (gltfMat.occlusionTexture.index != -1)
                    mat->textureIndices[GLTFTextureType::Occlusion] = gltfMat.occlusionTexture.index;

                if (gltfMat.pbrMetallicRoughness.baseColorTexture.index != -1)
                    mat->textureIndices[GLTFTextureType::BaseColor] = gltfMat.pbrMetallicRoughness.baseColorTexture.index;

                if (gltfMat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
                    mat->textureIndices[GLTFTextureType::MetallicRoughness] = gltfMat.pbrMetallicRoughness.metallicRoughnessTexture.index;
            }
        }

        if (!model.textures.empty())
        {
            outData.allTextures      = new ModelTexture[model.textures.size()];
            outData.allTexturesCount = static_cast<uint32>(model.textures.size());

            for (uint32 i = 0; i < outData.allTexturesCount; i++)
            {
                const auto&   gltfTexture = model.textures[i];
                ModelTexture* texture     = outData.allTextures + i;
                texture->name             = gltfTexture.name;

                if (gltfTexture.source != -1)
                {
                    auto& image = model.images[gltfTexture.source];
                    LOGA((image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE), "Unsupported pixel type!");

                    texture->buffer.pixels        = new unsigned char[image.image.size()];
                    texture->buffer.width         = image.width;
                    texture->buffer.height        = image.height;
                    texture->buffer.bytesPerPixel = image.bits / 8 * image.component;

                    std::memcpy(texture->buffer.pixels, image.image.data(), image.image.size());
                    const unsigned char* pixelData = image.image.data();
                }
            }
        }

        outData.allMeshes      = new ModelMesh[model.meshes.size()];
        outData.allMeshesCount = static_cast<uint32>(model.meshes.size());

        for (size_t i = 0; i < model.nodes.size(); i++)
        {
            const auto& gltfNode = model.nodes[i];
            ModelNode*  node     = outData.allNodes + i;
            node->name           = gltfNode.name.c_str();

            if (!gltfNode.matrix.empty())
            {
                LOGA(gltfNode.matrix.size() == 16, "Unsupported matrix type!");

                for (uint32 j = 0; j < 16; j++)
                    node->localMatrix.values[j] = static_cast<float>(gltfNode.matrix[j]);
            }
            else
            {
                if (!gltfNode.translation.empty())
                {
                    node->position.x = static_cast<float>(gltfNode.translation[0]);
                    node->position.y = static_cast<float>(gltfNode.translation[1]);
                    node->position.z = static_cast<float>(gltfNode.translation[2]);
                }

                if (!gltfNode.scale.empty())
                {
                    node->scale.x = static_cast<float>(gltfNode.scale[0]);
                    node->scale.y = static_cast<float>(gltfNode.scale[1]);
                    node->scale.z = static_cast<float>(gltfNode.scale[2]);
                }

                if (!gltfNode.rotation.empty())
                {
                    node->quatRot.x = static_cast<float>(gltfNode.rotation[0]);
                    node->quatRot.y = static_cast<float>(gltfNode.rotation[1]);
                    node->quatRot.z = static_cast<float>(gltfNode.rotation[2]);
                    node->quatRot.w = static_cast<float>(gltfNode.rotation[3]);
                }

                node->localMatrix = Matrix4::Identity();
                node->localMatrix = Matrix4::Translate(node->localMatrix, node->position);
                node->localMatrix = Matrix4::Rotate(node->localMatrix, node->quatRot);
                node->localMatrix = Matrix4::Scale(node->localMatrix, node->scale);

                // node->localMatrix.InitTranslationRotationScale(node->position, node->quatRot, node->scale);
            }

            if (!gltfNode.children.empty())
            {
                node->children.resize(gltfNode.children.size());

                for (size_t j = 0; j < gltfNode.children.size(); j++)
                {
                    node->children[j]         = outData.allNodes + gltfNode.children[j];
                    node->children[j]->parent = node;
                }
            }

            // Process the rest of the node.
            if (gltfNode.mesh != -1)
            {
                const auto& tgMesh         = model.meshes[gltfNode.mesh];
                node->mesh                 = outData.allMeshes + gltfNode.mesh;
                node->mesh->node           = node;
                node->mesh->name           = tgMesh.name;
                node->mesh->primitiveCount = static_cast<uint32>(tgMesh.primitives.size());
                node->mesh->primitives     = new ModelMeshPrimitive[tgMesh.primitives.size()];

                uint32 primitiveIndex = 0;
                for (const auto& tgPrimitive : tgMesh.primitives)
                {
                    ModelMeshPrimitive* primitive = node->mesh->primitives + primitiveIndex;

                    const tinygltf::Accessor&   vertexAccessor   = model.accessors[tgPrimitive.attributes.find("POSITION")->second];
                    const tinygltf::BufferView& vertexBufferView = model.bufferViews[vertexAccessor.bufferView];
                    const tinygltf::Buffer&     vertexBuffer     = model.buffers[vertexBufferView.buffer];

                    LOGA((vertexAccessor.type == TINYGLTF_TYPE_VEC3 && vertexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

                    const size_t numVertices = vertexAccessor.count;
                    primitive->positions.resize(numVertices);
                    primitive->vertexCount = static_cast<uint32>(numVertices);

                    for (size_t j = 0; j < numVertices; ++j)
                    {
                        const size_t stride       = vertexBufferView.byteStride == 0 ? sizeof(float) * 3 : vertexBufferView.byteStride;
                        const float* rawFloatData = reinterpret_cast<const float*>(vertexBuffer.data.data() + vertexAccessor.byteOffset + vertexBufferView.byteOffset + j * stride);
                        Vector3&     position     = primitive->positions[j];
                        position.x                = rawFloatData[0];
                        position.y                = rawFloatData[1];
                        position.z                = rawFloatData[2];
                    }

                    if (tgPrimitive.indices != -1)
                    {
                        const tinygltf::Accessor&   indexAccessor   = model.accessors[tgPrimitive.indices];
                        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                        const tinygltf::Buffer&     indexBuffer     = model.buffers[indexBufferView.buffer];
                        LOGA((indexAccessor.type == TINYGLTF_TYPE_SCALAR && vertexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_INT), "Unsupported componet type!");

                        const size_t numIndices = indexBufferView.byteLength / indexBufferView.byteStride;
                        primitive->indices.resize(numIndices);
                        for (size_t j = 0; j < numIndices; j++)
                        {
                            const size_t  stride  = indexBufferView.byteStride == 0 ? sizeof(int) : indexBufferView.byteStride;
                            const uint32* rawData = reinterpret_cast<const uint32*>(indexBuffer.data.data() + indexAccessor.byteOffset + indexBufferView.byteOffset + j * stride);
                            uint32&       index   = primitive->indices[j];
                            index                 = rawData[0];
                        }
                    }

                    auto normalsAttribute = tgPrimitive.attributes.find("NORMAL");
                    if (normalsAttribute != tgPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor&   normalsAccessor   = model.accessors[normalsAttribute->second];
                        const tinygltf::BufferView& normalsBufferView = model.bufferViews[normalsAccessor.bufferView];
                        const tinygltf::Buffer&     normalsBuffer     = model.buffers[normalsBufferView.buffer];
                        LOGA((normalsAccessor.type == TINYGLTF_TYPE_VEC3 && normalsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

                        const size_t numNormals = normalsAccessor.count;
                        primitive->normals.resize(numNormals);

                        for (size_t j = 0; j < numNormals; ++j)
                        {
                            const size_t stride       = normalsBufferView.byteStride == 0 ? sizeof(float) * 3 : normalsBufferView.byteStride;
                            const float* rawFloatData = reinterpret_cast<const float*>(normalsBuffer.data.data() + normalsAccessor.byteOffset + normalsBufferView.byteOffset + j * stride);
                            Vector3&     normal       = primitive->normals[j];
                            normal.x                  = rawFloatData[0];
                            normal.y                  = rawFloatData[1];
                            normal.z                  = rawFloatData[2];
                        }
                    }

                    auto colorsAttribute = tgPrimitive.attributes.find("COLOR_0");
                    if (colorsAttribute != tgPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor&   colorsAccessor   = model.accessors[colorsAttribute->second];
                        const tinygltf::BufferView& colorsBufferView = model.bufferViews[colorsAccessor.bufferView];
                        const tinygltf::Buffer&     colorsBuffer     = model.buffers[colorsBufferView.buffer];
                        LOGA((colorsAccessor.type == TINYGLTF_TYPE_VEC4 && colorsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

                        const size_t numColors = colorsAccessor.count;
                        primitive->colors.resize(numColors);

                        for (size_t j = 0; j < numColors; ++j)
                        {
                            const size_t stride       = colorsBufferView.byteStride == 0 ? sizeof(float) * 4 : colorsBufferView.byteStride;
                            const float* rawFloatData = reinterpret_cast<const float*>(colorsBuffer.data.data() + colorsAccessor.byteOffset + colorsBufferView.byteOffset + j * stride);
                            Vector4&     color        = primitive->colors[j];
                            color.x                   = rawFloatData[0];
                            color.y                   = rawFloatData[1];
                            color.z                   = rawFloatData[2];
                            color.w                   = rawFloatData[3];
                        }
                    }

                    auto tangentsAttribute = tgPrimitive.attributes.find("tangent_0");
                    if (tangentsAttribute != tgPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor&   tangentsAccessor   = model.accessors[tangentsAttribute->second];
                        const tinygltf::BufferView& tangentsBufferView = model.bufferViews[tangentsAccessor.bufferView];
                        const tinygltf::Buffer&     tangentsBuffer     = model.buffers[tangentsBufferView.buffer];
                        LOGA((tangentsAccessor.type == TINYGLTF_TYPE_VEC4 && tangentsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

                        const size_t numTangents = tangentsAccessor.count;
                        primitive->tangents.resize(numTangents);

                        for (size_t j = 0; j < numTangents; ++j)
                        {
                            const size_t stride       = tangentsBufferView.byteStride == 0 ? sizeof(float) * 4 : tangentsBufferView.byteStride;
                            const float* rawFloatData = reinterpret_cast<const float*>(tangentsBuffer.data.data() + tangentsAccessor.byteOffset + tangentsBufferView.byteOffset + j * stride);
                            Vector4&     tangent      = primitive->tangents[j];
                            tangent.x                 = rawFloatData[0];
                            tangent.y                 = rawFloatData[1];
                            tangent.z                 = rawFloatData[2];
                            tangent.w                 = rawFloatData[3];
                        }
                    }

                    auto texcoordsAttribute = tgPrimitive.attributes.find("TEXCOORD_0");
                    if (texcoordsAttribute != tgPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor&   texcoordAccessor   = model.accessors[texcoordsAttribute->second];
                        const tinygltf::BufferView& texcoordBufferView = model.bufferViews[texcoordAccessor.bufferView];
                        const tinygltf::Buffer&     texcoordBuffer     = model.buffers[texcoordBufferView.buffer];
                        LOGA((texcoordAccessor.type == TINYGLTF_TYPE_VEC2 && texcoordAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

                        const size_t numCoords = texcoordAccessor.count;
                        primitive->texCoords.resize(numCoords);

                        for (size_t j = 0; j < numCoords; ++j)
                        {
                            const size_t stride       = texcoordBufferView.byteStride == 0 ? sizeof(float) * 2 : texcoordBufferView.byteStride;
                            const float* rawFloatData = reinterpret_cast<const float*>(texcoordBuffer.data.data() + texcoordAccessor.byteOffset + texcoordBufferView.byteOffset + j * stride);
                            Vector2&     coord        = primitive->texCoords[j];
                            coord.x                   = rawFloatData[0];
                            coord.y                   = rawFloatData[1];
                        }
                    }

                    auto joints0 = tgPrimitive.attributes.find("JOINTS_0");
                    if (joints0 != tgPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor&   jointsAccessor   = model.accessors[joints0->second];
                        const tinygltf::BufferView& jointsBufferView = model.bufferViews[jointsAccessor.bufferView];
                        const tinygltf::Buffer&     jointsBuffer     = model.buffers[jointsBufferView.buffer];
                        LOGA((jointsAccessor.type == TINYGLTF_TYPE_VEC4 && jointsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT), "Unsupported component type!");

                        const size_t numJoints = jointsAccessor.count;
                        primitive->joints.resize(numJoints);

                        for (size_t j = 0; j < numJoints; j++)
                        {
                            const size_t  stride  = jointsBufferView.byteStride == 0 ? sizeof(uint16) * 4 : jointsBufferView.byteStride;
                            const uint16* rawData = reinterpret_cast<const uint16*>(jointsBuffer.data.data() + jointsAccessor.byteOffset + jointsBufferView.byteOffset + j * stride);
                            primitive->joints[j]  = {rawData[0], rawData[1], rawData[2], rawData[3]};
                        }
                    }

                    auto weights0 = tgPrimitive.attributes.find("WEIGHTS_0");
                    if (weights0 != tgPrimitive.attributes.end())
                    {
                        const tinygltf::Accessor&   weightsAccessor   = model.accessors[weights0->second];
                        const tinygltf::BufferView& weightsBufferView = model.bufferViews[weightsAccessor.bufferView];
                        const tinygltf::Buffer&     weightsBuffer     = model.buffers[weightsBufferView.buffer];
                        LOGA((weightsAccessor.type == TINYGLTF_TYPE_VEC4 && weightsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

                        const size_t numWeights = weightsAccessor.count;
                        primitive->weights.resize(numWeights);

                        for (size_t j = 0; j < numWeights; ++j)
                        {
                            const size_t stride   = weightsBufferView.byteStride == 0 ? sizeof(float) * 4 : weightsBufferView.byteStride;
                            const float* rawData  = reinterpret_cast<const float*>(weightsBuffer.data.data() + weightsAccessor.byteOffset + weightsBufferView.byteOffset + j * stride);
                            primitive->weights[j] = {rawData[0], rawData[1], rawData[2], rawData[3]};
                        }
                    }

                    primitiveIndex++;
                }
            }
        }

        outData.allSkins      = new ModelSkin[model.skins.size()];
        outData.allSkinsCount = static_cast<uint32>(model.skins.size());

        for (size_t i = 0; i < model.skins.size(); i++)
        {
            const auto& gltfSkin = model.skins[i];
            ModelSkin*  skin     = outData.allSkins + i;

            skin->joints.resize(gltfSkin.joints.size());

            for (size_t j = 0; j < gltfSkin.joints.size(); j++)
                skin->joints[j] = outData.allNodes + gltfSkin.joints[j];

            skin->rootJoint = outData.allNodes + gltfSkin.skeleton;

            const tinygltf::Accessor&   inverseBindMatricesAccessor = model.accessors[gltfSkin.inverseBindMatrices];
            const tinygltf::BufferView& inverseBindMatricesView     = model.bufferViews[inverseBindMatricesAccessor.bufferView];
            const tinygltf::Buffer&     inverseBindMatricesBuffer   = model.buffers[inverseBindMatricesView.buffer];
            const size_t                count                       = inverseBindMatricesAccessor.count;
            LOGA((inverseBindMatricesAccessor.type == TINYGLTF_TYPE_MAT4 && inverseBindMatricesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");

            for (size_t j = 0; j < count; j++)
            {
                const size_t stride       = inverseBindMatricesView.byteStride == 0 ? sizeof(float) * 16 : inverseBindMatricesView.byteStride;
                const float* rawFloatData = reinterpret_cast<const float*>(inverseBindMatricesBuffer.data.data() + inverseBindMatricesAccessor.byteOffset + inverseBindMatricesView.byteOffset + j * stride);

                for (size_t k = 0; k < 16; k++)
                    skin->joints[j]->inverseBindMatrix.values[k] = rawFloatData[k];
            }
        }

        for (size_t i = 0; i < model.nodes.size(); i++)
        {
            ModelNode* node = outData.allNodes + i;

            // Calculate all matrices.
            node->globalMatrix = CalculateGlobalMatrix(node);

            // Assign skin.
            if (model.nodes[i].skin != -1)
                node->skin = outData.allSkins + model.nodes[i].skin;

            // Assign roots.
            if (node->parent == nullptr)
                outData.rootNodes.push_back(node);
        }

        outData.allAnimations      = new ModelAnimation[model.animations.size()];
        outData.allAnimationsCount = static_cast<uint32>(model.animations.size());

        for (size_t i = 0; i < model.animations.size(); i++)
        {
            ModelAnimation*            anim          = outData.allAnimations + i;
            const tinygltf::Animation& gltfAnimation = model.animations[i];
            anim->name                               = gltfAnimation.name;

            for (const auto& ch : gltfAnimation.channels)
            {
                anim->channels.push_back({});
                ModelAnimationChannel& channel = anim->channels[anim->channels.size() - 1];
                channel.targetNode             = outData.allNodes + ch.target_node;

                if (ch.target_path.compare("translation") == 0)
                    channel.targetProperty = GLTFAnimationProperty::Position;
                else if (ch.target_path.compare("rotation") == 0)
                    channel.targetProperty = GLTFAnimationProperty::Rotation;
                else if (ch.target_path.compare("scale") == 0)
                    channel.targetProperty = GLTFAnimationProperty::Scale;
                else if (ch.target_path.compare("weights") == 0)
                    channel.targetProperty = GLTFAnimationProperty::Weights;

                const tinygltf::AnimationSampler sampler = gltfAnimation.samplers[ch.sampler];

                if (sampler.interpolation.compare("LINEAR") == 0)
                    channel.interpolation = GLTFInterpolation::Linear;
                else if (sampler.interpolation.compare("STEP") == 0)
                    channel.interpolation = GLTFInterpolation::Step;
                else if (sampler.interpolation.compare("CUBICSPLINE") == 0)
                    channel.interpolation = GLTFInterpolation::CubicSpline;

                const tinygltf::Accessor&   inputAccessor  = model.accessors[sampler.input];
                const tinygltf::BufferView& inputView      = model.bufferViews[inputAccessor.bufferView];
                const tinygltf::Buffer&     inputBuffer    = model.buffers[inputView.buffer];
                const tinygltf::Accessor&   outputAccessor = model.accessors[sampler.output];
                const tinygltf::BufferView& outputView     = model.bufferViews[outputAccessor.bufferView];
                const tinygltf::Buffer&     outputBuffer   = model.buffers[outputView.buffer];

                LOGA((inputAccessor.type == TINYGLTF_TYPE_SCALAR && inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT), "Unsupported component type!");
                LOGA(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, "Unsupported component type!");

                const size_t inputCount  = inputAccessor.count;
                const size_t outputCount = outputAccessor.count;
                LOGA(inputCount == outputCount, "Input & output counts do not match!");

                channel.keyframeTimes.resize(inputCount);

                for (size_t j = 0; j < inputCount; j++)
                {
                    const size_t stride       = inputView.byteStride == 0 ? sizeof(float) : inputView.byteStride;
                    const float* rawFloatData = reinterpret_cast<const float*>(inputBuffer.data.data() + inputAccessor.byteOffset + inputView.byteOffset + j * stride);
                    channel.keyframeTimes[j]  = rawFloatData[0];
                }

                const float* rawFloatData = reinterpret_cast<const float*>(outputBuffer.data.data() + outputAccessor.byteOffset + outputView.byteOffset);

                // TODO
                const uint32 numMorphTargets = 1;

                switch (channel.targetProperty)
                {
                case GLTFAnimationProperty::Position:
                case GLTFAnimationProperty::Scale:
                    channel.values.assign(rawFloatData, rawFloatData + outputCount * 3);
                    break;
                case GLTFAnimationProperty::Rotation:
                    channel.values.assign(rawFloatData, rawFloatData + outputCount * 4);
                    break;
                case GLTFAnimationProperty::Weights:
                    channel.values.assign(rawFloatData, rawFloatData + outputCount * numMorphTargets);
                    break;
                default:
                    // Handle error
                    break;
                }
            }
        }
        int a = 5;
    }

    LINAGX_API void LoadGLTFBinary(const char* path, ModelData& outData)
    {
        tinygltf::Model    model;
        tinygltf::TinyGLTF loader;

        LINAGX_STRING err  = "";
        LINAGX_STRING warn = "";

        bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);

        if (!warn.empty())
        {
            LOGE("Warning -> %s", warn.c_str());
        }

        if (!err.empty())
        {
            LOGE("Error -> %s", err.c_str());
        }

        if (!ret)
        {
            LOGE("Loading GLTF from binary failed! %s", path);
            return;
        }

        ProcessGLTF(model, outData);

        LOGV("Loaded GLTF binary: %s", path);
    }

    LINAGX_API void LoadGLTFASCII(const char* path, ModelData& outData)
    {
        tinygltf::Model    model;
        tinygltf::TinyGLTF loader;

        LINAGX_STRING err  = "";
        LINAGX_STRING warn = "";

        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

        if (!warn.empty())
        {
            LOGE("Warning -> %s", warn.c_str());
        }

        if (!err.empty())
        {
            LOGE("Error -> %s", err.c_str());
        }

        if (!ret)
        {
            LOGE("Loading GLTF from binary failed! %s", path);
            return;
        }

        ProcessGLTF(model, outData);

        LOGV("Loaded GLTF binary: %s", path);
    }
} // namespace LinaGX
