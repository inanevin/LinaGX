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
                mat->metallicFactor    = gltfMat.pbrMetallicRoughness.metallicFactor;
                mat->roughnessFactor   = gltfMat.pbrMetallicRoughness.roughnessFactor;
                mat->emissive          = {static_cast<float>(gltfMat.emissiveFactor[0]), static_cast<float>(gltfMat.emissiveFactor[1]), static_cast<float>(gltfMat.emissiveFactor[2])};
                mat->baseColor         = {static_cast<float>(gltfMat.pbrMetallicRoughness.baseColorFactor[0]), static_cast<float>(gltfMat.pbrMetallicRoughness.baseColorFactor[1]), static_cast<float>(gltfMat.pbrMetallicRoughness.baseColorFactor[2]), static_cast<float>(gltfMat.pbrMetallicRoughness.baseColorFactor[3])};
                mat->alphaCutoff       = static_cast<float>(gltfMat.alphaCutoff);
                mat->isOpaque          = gltfMat.alphaMode.compare("OPAQUE") == 0;
                mat->doubleSided       = gltfMat.doubleSided;

                if (gltfMat.emissiveTexture.index != -1)
                    mat->textureIndices[LINAGX_TEXTURE_EMISSIVE] = gltfMat.emissiveTexture.index;

                if (gltfMat.normalTexture.index != -1)
                    mat->textureIndices[LINAGX_TEXTURE_NORMAL] = gltfMat.normalTexture.index;

                if (gltfMat.occlusionTexture.index != -1)
                    mat->textureIndices[LINAGX_TEXTURE_OCCLUSION] = gltfMat.occlusionTexture.index;

                if (gltfMat.pbrMetallicRoughness.baseColorTexture.index != -1)
                    mat->textureIndices[LINAGX_TEXTURE_BASECOLOR] = gltfMat.pbrMetallicRoughness.baseColorTexture.index;

                if (gltfMat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
                    mat->textureIndices[LINAGX_TEXTURE_METALLIC_ROUGHNESS] = gltfMat.pbrMetallicRoughness.metallicRoughnessTexture.index;
            }
        }

        if (!model.textures.empty())
        {
            outData.allTextures      = new TextureBuffer[model.textures.size()];
            outData.allTexturesCount = static_cast<uint32>(model.textures.size());

            for (uint32 i = 0; i < outData.allTexturesCount; i++)
            {
                TextureBuffer* buf        = outData.allTextures + i;
                int            imageIndex = model.textures[i].source;

                if (imageIndex != -1)
                {
                    auto& image = model.images[imageIndex];

                    buf->pixels        = new unsigned char[image.image.size()];
                    buf->width         = image.width;
                    buf->height        = image.height;
                    buf->bytesPerPixel = image.bits / 8 * image.component;

                    std::memcpy(buf->pixels, image.image.data(), image.image.size());
                    const unsigned char* pixelData = image.image.data();
                }
            }
        }

        for (size_t i = 0; i < model.nodes.size(); i++)
        {
            const auto& gltfNode = model.nodes[i];
            ModelNode*  node     = outData.allNodes + i;

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
                node->mesh                 = new ModelMesh();
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
                    primitive->vertices.resize(numVertices);

                    for (size_t i = 0; i < numVertices; ++i)
                    {
                        const float*    rawFloatData = reinterpret_cast<const float*>(vertexBuffer.data.data() + vertexBufferView.byteOffset + i * vertexBufferView.byteStride);
                        LINAGX_VECTOR3& position     = primitive->vertices[i];
                        position.x                   = rawFloatData[0];
                        position.y                   = rawFloatData[1];
                        position.z                   = rawFloatData[2];
                    }

                    if (tgPrimitive.indices != -1)
                    {
                        const tinygltf::Accessor&   indexAccessor   = model.accessors[tgPrimitive.indices];
                        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                        const tinygltf::Buffer&     indexBuffer     = model.buffers[indexBufferView.buffer];
                        LOGA((indexAccessor.type == TINYGLTF_TYPE_SCALAR && vertexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_INT), "Unsupported componet type!");

                        const size_t numIndices = indexBufferView.byteLength / indexBufferView.byteStride;
                        primitive->indices.resize(numIndices);
                        for (size_t i = 0; i < numIndices; i++)
                        {
                            const uint32* rawData = reinterpret_cast<const uint32*>(indexBuffer.data.data() + indexBufferView.byteOffset + i * indexBufferView.byteStride);
                            uint32&       index   = primitive->indices[i];
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

                        for (size_t i = 0; i < numNormals; ++i)
                        {
                            const float*    rawFloatData = reinterpret_cast<const float*>(normalsBuffer.data.data() + normalsBufferView.byteOffset + i * normalsBufferView.byteStride);
                            LINAGX_VECTOR3& normal       = primitive->normals[i];
                            normal.x                     = rawFloatData[0];
                            normal.y                     = rawFloatData[1];
                            normal.z                     = rawFloatData[2];
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

                        for (size_t i = 0; i < numColors; ++i)
                        {
                            const float*    rawFloatData = reinterpret_cast<const float*>(colorsBuffer.data.data() + colorsBufferView.byteOffset + i * colorsBufferView.byteStride);
                            LINAGX_VECTOR4& color        = primitive->colors[i];
                            color.x                      = rawFloatData[0];
                            color.y                      = rawFloatData[1];
                            color.z                      = rawFloatData[2];
                            color.w                      = rawFloatData[3];
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

                        for (size_t i = 0; i < numTangents; ++i)
                        {
                            const float*    rawFloatData = reinterpret_cast<const float*>(tangentsBuffer.data.data() + tangentsBufferView.byteOffset + i * tangentsBufferView.byteStride);
                            LINAGX_VECTOR4& tangent      = primitive->tangents[i];
                            tangent.x                    = rawFloatData[0];
                            tangent.y                    = rawFloatData[1];
                            tangent.z                    = rawFloatData[2];
                            tangent.w                    = rawFloatData[3];
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

                        for (size_t i = 0; i < numCoords; ++i)
                        {
                            const float*    rawFloatData = reinterpret_cast<const float*>(texcoordBuffer.data.data() + texcoordBufferView.byteOffset + i * texcoordBufferView.byteStride);
                            LINAGX_VECTOR2& coord        = primitive->texCoords[i];
                            coord.x                      = rawFloatData[0];
                            coord.y                      = rawFloatData[1];
                            int a                        = 5;
                        }
                    }

                    primitiveIndex++;
                }
            }
        }

        for (size_t i = 0; i < model.nodes.size(); i++)
        {
            ModelNode* node = outData.allNodes + i;
            if (node->parent == nullptr)
                outData.rootNodes.push_back(node);
        }
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
