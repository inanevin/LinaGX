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

#include "LinaGX/Platform/Metal/MTLBackend.hpp"
#include "LinaGX/Utility/PlatformUtility.hpp"
#include "LinaGX/Core/Commands.hpp"
#include "LinaGX/Core/Instance.hpp"
#include "LinaGX/Core/CommandStream.hpp"
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Cocoa/Cocoa.h>


namespace LinaGX
{

#define AS_MTL(X,Y) static_cast<Y>(X)
#define AS_VOID(X) static_cast<void*>(X)

#ifndef NDEBUG

#define NAME_OBJ_CSTR(X,Y) NSString *debugNSString = [NSString stringWithUTF8String:Y]; \
[X setLabel:debugNSString]

#else

#define NAME_OBJ_CSTR(X,Y)

#endif

MTLPixelFormat GetMTLFormat(Format format)
{
    switch (format)
    {
    case Format::UNDEFINED:
        return MTLPixelFormatInvalid;
    case Format::B8G8R8A8_SRGB:
        return MTLPixelFormatBGRA8Unorm_sRGB;
    case Format::B8G8R8A8_UNORM:
        return MTLPixelFormatBGRA8Unorm;
    case Format::R32G32B32_SFLOAT:
        return MTLPixelFormatInvalid;
    case Format::R32G32B32_SINT:
        return MTLPixelFormatInvalid;
    case Format::R32G32B32A32_SFLOAT:
        return MTLPixelFormatRGBA32Float;
    case Format::R32G32B32A32_SINT:
        return MTLPixelFormatRGBA32Sint;
    case Format::R16G16B16A16_SFLOAT:
        return MTLPixelFormatRGBA16Float;
    case Format::R16G16B16A16_UNORM:
        return MTLPixelFormatRGBA16Unorm;
    case Format::R32G32_SFLOAT:
        return MTLPixelFormatRG32Float;
    case Format::R32G32_SINT:
        return MTLPixelFormatRG32Sint;
    case Format::D32_SFLOAT:
        return MTLPixelFormatDepth32Float;
    case Format::R8G8B8A8_UNORM:
        return MTLPixelFormatRGBA8Unorm;
    case Format::R8G8B8A8_SRGB:
        return MTLPixelFormatRGBA8Unorm_sRGB;
    case Format::R8G8_UNORM:
        return MTLPixelFormatRG8Unorm;
    case Format::R8_UNORM:
        return MTLPixelFormatR8Unorm;
    case Format::R8_UINT:
        return MTLPixelFormatR8Uint;
    case Format::R16_SFLOAT:
        return MTLPixelFormatR16Float;
    case Format::R16_SINT:
        return MTLPixelFormatR16Sint;
    case Format::R32_SFLOAT:
        return MTLPixelFormatR32Float;
    case Format::R32_SINT:
        return MTLPixelFormatR32Sint;
    case Format::R32_UINT:
        return MTLPixelFormatR32Uint;
    default:
        return MTLPixelFormatInvalid;
    }

    return MTLPixelFormatInvalid;
}

MTLBlendFactor GetMTLBlendFactor(BlendFactor factor)
{
    switch (factor)
    {
        case BlendFactor::Zero:
            return MTLBlendFactorZero;
        case BlendFactor::One:
            return MTLBlendFactorOne;
        case BlendFactor::SrcColor:
            return MTLBlendFactorSourceColor;
        case BlendFactor::OneMinusSrcColor:
            return MTLBlendFactorOneMinusSourceColor;
        case BlendFactor::DstColor:
            return MTLBlendFactorDestinationColor;
        case BlendFactor::OneMinusDstColor:
            return MTLBlendFactorOneMinusDestinationColor;
        case BlendFactor::SrcAlpha:
            return MTLBlendFactorSourceAlpha;
        case BlendFactor::OneMinusSrcAlpha:
            return MTLBlendFactorOneMinusSourceAlpha;
        case BlendFactor::DstAlpha:
            return MTLBlendFactorDestinationAlpha;
        case BlendFactor::OneMinusDstAlpha:
            return MTLBlendFactorOneMinusDestinationAlpha;
        default:
            return MTLBlendFactorZero;
    }
}

MTLBlendOperation GetMTLBlendOp(BlendOp op) {
    switch (op) {
        case BlendOp::Add:
            return MTLBlendOperationAdd;
        case BlendOp::Subtract:
            return MTLBlendOperationSubtract;
        case BlendOp::ReverseSubtract:
            return MTLBlendOperationReverseSubtract;
        case BlendOp::Min:
            return MTLBlendOperationMin;
        case BlendOp::Max:
            return MTLBlendOperationMax;
        default:
            return MTLBlendOperationAdd;
    }
}

MTLCompareFunction GetMTLCompareOp(CompareOp op) {
    switch (op) {
        case CompareOp::Never:
            return MTLCompareFunctionNever;
        case CompareOp::Less:
            return MTLCompareFunctionLess;
        case CompareOp::Equal:
            return MTLCompareFunctionEqual;
        case CompareOp::LEqual:
            return MTLCompareFunctionLessEqual;
        case CompareOp::Greater:
            return MTLCompareFunctionGreater;
        case CompareOp::NotEqual:
            return MTLCompareFunctionNotEqual;
        case CompareOp::GEqual:
            return MTLCompareFunctionGreaterEqual;
        case CompareOp::Always:
            return MTLCompareFunctionAlways;
        default:
            return MTLCompareFunctionAlways;
    }
}

MTLVertexFormat GetMTLVertexFormat(Format format)
{
    switch(format)
    {
        case Format::R32G32B32A32_SFLOAT:
            return MTLVertexFormatFloat4;
        case Format::R32G32B32A32_SINT:
            return MTLVertexFormatInt4;
        case Format::R32G32B32_SFLOAT:
            return MTLVertexFormatFloat3;
        case Format::R32G32B32_SINT:
            return MTLVertexFormatInt3;
        case Format::R32G32_SFLOAT:
            return MTLVertexFormatFloat2;
        case Format::R32G32_SINT:
            return MTLVertexFormatInt2;
        case Format::R32_SINT:
            return MTLVertexFormatInt;
        case Format::R32_SFLOAT:
            return MTLVertexFormatFloat;
        default:
            LOGA(false, "Not supported yet!");
            return MTLVertexFormatInvalid;
    }
}

MTLCullMode GetMTLCullMode(CullMode mode)
{
        switch(mode)
        {
            case CullMode::Back:
                return MTLCullModeBack;
            case CullMode::Front:
                return MTLCullModeFront;
            case CullMode::None:
                return MTLCullModeNone;
            default:
                return MTLCullModeNone;
        }
}

MTLPrimitiveTopologyClass GetMTLTopology(Topology topology)
{
    switch(topology)
    {
        case Topology::PointList:
            return MTLPrimitiveTopologyClassPoint;
        case Topology::LineList:
        case Topology::LineStrip:
            return MTLPrimitiveTopologyClassLine;
        case Topology::TriangleList:
        case Topology::TriangleStrip:
        case Topology::TriangleFan:
        case Topology::TriangleListAdjacency:
        case Topology::TriangleStripAdjacency:
        default:
            return MTLPrimitiveTopologyClassTriangle;
    }
}

MTLPrimitiveType GetMTLPrimitive(Topology topology)
{
    switch(topology)
    {
        case Topology::PointList:
            return MTLPrimitiveTypePoint;
        case Topology::LineList:
            return MTLPrimitiveTypeLine;
        case Topology::LineStrip:
            return MTLPrimitiveTypeLineStrip;
        case Topology::TriangleStrip:
            return MTLPrimitiveTypeTriangleStrip;
        case Topology::TriangleFan:
        case Topology::TriangleList:
        case Topology::TriangleListAdjacency:
        case Topology::TriangleStripAdjacency:
        default:
            return MTLPrimitiveTypeTriangle;
    }
}

MTLSamplerMinMagFilter GetMTLMinMagFilter(Filter filter)
{
    switch(filter)
    {
        case Filter::Anisotropic:
        case Filter::Linear:
            return MTLSamplerMinMagFilterLinear;
        case Filter::Nearest:
            return MTLSamplerMinMagFilterNearest;
            
    }
}

MTLSamplerAddressMode GetMTLSamplerAddressMode(SamplerAddressMode mode)
{
    switch(mode)
    {
        case SamplerAddressMode::ClampToEdge:
            return MTLSamplerAddressModeClampToEdge;
        case SamplerAddressMode::ClampToBorder:
            return MTLSamplerAddressModeClampToBorderColor;
        case SamplerAddressMode::MirrorClampToEdge:
            return MTLSamplerAddressModeMirrorClampToEdge;
        case SamplerAddressMode::Repeat:
            return MTLSamplerAddressModeRepeat;
        case SamplerAddressMode::MirroredRepeat:
            return MTLSamplerAddressModeMirrorRepeat;
            
    }
}

MTLSamplerMipFilter GetMTLMipFilter(MipmapMode mode)
{
    switch(mode)
    {
        case MipmapMode::Linear:
            return MTLSamplerMipFilterLinear;
        case MipmapMode::Nearest:
            return MTLSamplerMipFilterNearest;
            
    }
}

MTLRenderStages GetMTLRenderStage(ShaderStage stage)
{
    switch(stage)
    {
        case ShaderStage::Fragment:
            return MTLRenderStageFragment;
        case ShaderStage::Vertex:
            return MTLRenderStageVertex;
        case ShaderStage::Tesellation:
        case ShaderStage::Geometry:
            LOGA(false, "Backend -> Not supported yet!");
            return MTLRenderStageVertex;
        case ShaderStage::Compute:
            return MTLRenderStageObject;
            
    }
}

MTLColorWriteMask GetMTLColorWriteMask(ColorComponentFlags flag)
{
        switch(flag)
        {
            case ColorComponentFlags::R:
                return MTLColorWriteMaskRed;
            case ColorComponentFlags::G:
                return MTLColorWriteMaskGreen;
            case ColorComponentFlags::B:
                return MTLColorWriteMaskBlue;
            case ColorComponentFlags::A:
                return MTLColorWriteMaskAlpha;
        }
}

MTLStencilOperation GetMTLStencilOperation(StencilOp op)
{
    switch(op)
    {
        case StencilOp::Keep:
            return MTLStencilOperationKeep;
        case StencilOp::Zero:
            return MTLStencilOperationZero;
        case StencilOp::Replace:
            return MTLStencilOperationReplace;
        case StencilOp::IncrementClamp:
            return MTLStencilOperationIncrementClamp;
        case StencilOp::DecrementClamp:
            return MTLStencilOperationDecrementClamp;
        case StencilOp::Invert:
            return MTLStencilOperationInvert;
        case StencilOp::IncrementWrap:
            return MTLStencilOperationIncrementWrap;
        case StencilOp::DecrementWrap:
            return MTLStencilOperationDecrementWrap;
    }
}

MTLLoadAction GetMTLLoadOp(LoadOp op)
{
    switch(op)
    {
        case LoadOp::DontCare:
            return MTLLoadActionDontCare;
        case LoadOp::Load:
            return MTLLoadActionLoad;
        case LoadOp::Clear:
            return MTLLoadActionClear;
        case LoadOp::None:
            return MTLLoadActionDontCare;
    }
}

MTLStoreAction GetMTLStoreOp(StoreOp op)
{
        switch(op)
        {
            case StoreOp::DontCare:
                return MTLStoreActionDontCare;
            case StoreOp::Store:
                return MTLStoreActionStore;
            case StoreOp::None:
                return MTLStoreActionUnknown;
        }
}

uint16 MTLBackend::CreateUserSemaphore() {
    MTLUserSemaphore item = {};
    item.isValid = true;
    
    auto device = AS_MTL(m_device, id<MTLDevice>);
    id<MTLSharedEvent> ev = [device newSharedEvent];
    [ev retain];
    item.semaphore = AS_VOID(ev);
    
    
    return m_userSemaphores.AddItem(item);
}

void MTLBackend::DestroyUserSemaphore(uint16 handle) {
    auto& semaphore = m_userSemaphores.GetItemR(handle);
    
    if (!semaphore.isValid)
    {
        LOGE("Backend -> Semaphore to be destroyed is not valid!");
        return;
    }
    
    id<MTLSharedEvent> ev = AS_MTL(semaphore.semaphore, id<MTLSharedEvent>);
    [ev release];

    m_userSemaphores.RemoveItem(handle);
}

void MTLBackend::WaitForUserSemaphore(uint16 handle, uint64 value)
{
    auto& semaphore = m_userSemaphores.GetItemR(handle);
    
    id<MTLSharedEvent> ev = AS_MTL(semaphore.semaphore, id<MTLSharedEvent>);
    
    while(ev.signaledValue < value)
    {
      
    }
    // waited.
}

uint8 MTLBackend::CreateSwapchain(const SwapchainDesc &desc) {
    MTLSwapchain item = {};
    item.isValid = true;
    
    CAMetalLayer* layer = [CAMetalLayer layer];
    [layer retain];
    layer.device = AS_MTL(m_device, id<MTLDevice>);
    layer.pixelFormat = GetMTLFormat(desc.format);
    layer.maximumDrawableCount = m_initInfo.backbufferCount;
    item.layer = AS_VOID(layer);
    item.width = desc.width;
    item.height = desc.height;
    
    NSWindow* wnd = AS_MTL(desc.window, NSWindow*);
    item.window = AS_VOID(wnd);
    
    CGSize sz;
    sz.width = desc.width;
    sz.height = desc.height;
    layer.drawableSize = sz;
    layer.contentsScale = wnd.backingScaleFactor;
    
    NSView* view = AS_MTL(desc.osHandle, NSView*);
    [view setWantsLayer:YES];
    [view setLayer:layer];
    // TODO: vsync.

   
    return m_swapchains.AddItem(item);
}

void MTLBackend::DestroySwapchain(uint8 handle) {
    auto& swp = m_swapchains.GetItemR(handle);
    if (!swp.isValid)
    {
        LOGE("Backend -> Swapchain to be destroyed is not valid!");
        return;
    }
    
    CAMetalLayer* layer = AS_MTL(swp.layer, CAMetalLayer*);
    [layer release];
 
    m_swapchains.RemoveItem(handle);
}

void MTLBackend::RecreateSwapchain(const SwapchainRecreateDesc &desc) {
    
    if (desc.width == 0 || desc.height == 0 || m_frameOnGoing)
        return;

    Join();
    
    LOGT("Recreating swapchain to new size %d %d", desc.width, desc.height);
    auto& swap = m_swapchains.GetItemR(desc.swapchain);
    swap.width = desc.width;
    swap.height = desc.height;
    CAMetalLayer* layer = AS_MTL(swap.layer, CAMetalLayer*);
    NSWindow* wnd = AS_MTL(swap.window, NSWindow*);
    CGSize sz;
    sz.width = desc.width;
    sz.height = desc.height;
    layer.drawableSize = sz;
    layer.contentsScale = wnd.backingScaleFactor;
    // TODO: vsync.
}

void MTLBackend::SetSwapchainActive(uint8 swp, bool isActive) {
    auto& swap = m_swapchains.GetItemR(swp);
    swap.isActive = isActive;
}

bool MTLBackend::CompileShader(ShaderStage stage, const std::string &source, DataBlob &outBlob) {
    auto device = AS_MTL(m_device, id<MTLDevice>);
    outBlob.ptr = (uint8*)LINAGX_MALLOC(source.size());
    outBlob.size = source.size();
    LINAGX_MEMCPY(outBlob.ptr, source.data(), source.size());
    return true;
}

uint16 MTLBackend::CreateShader(const ShaderDesc &shaderDesc) {
    MTLShader item = {};
    item.isValid = true;
    item.polygonMode = shaderDesc.polygonMode;
    item.cullMode = shaderDesc.cullMode;
    item.frontFace = shaderDesc.frontFace;
    item.topology = shaderDesc.topology;
    item.layout = shaderDesc.layout;
    
    for (const auto& [stage, blob] : shaderDesc.stages)
    {
        if (stage == ShaderStage::Compute)
        {
            if (shaderDesc.stages.size() == 1)
            {
                item.isCompute = true;
                break;
            }
            else
            {
                LOGA(false, "Shader contains a compute stage but that is not the only stage, which is forbidden!");
            }
        }
    }
    
    auto device = AS_MTL(m_device, id<MTLDevice>);
    
    if(item.isCompute)
    {
        const auto& blob = shaderDesc.stages.at(ShaderStage::Compute);
        
        NSString* src = [[NSString alloc] initWithBytes:blob.ptr length:blob.size encoding:NSUTF8StringEncoding];
        NSError* err = nil;

        id<MTLLibrary> lib = [device newLibraryWithSource:src options:nil error:&err];
        [lib retain];
        
        NSString* entryPoint = [NSString stringWithUTF8String:shaderDesc.layout.entryPoints.at(ShaderStage::Compute).c_str()];
        id<MTLFunction> computeFunc = [lib newFunctionWithName:entryPoint];
        [computeFunc retain];

        MTLComputePipelineDescriptor *computeDesc = [[MTLComputePipelineDescriptor alloc] init];
        NAME_OBJ_CSTR(computeDesc, shaderDesc.debugName);
        computeDesc.computeFunction = computeFunc;
        
        MTLAutoreleasedComputePipelineReflection ref;
        id<MTLComputePipelineState> cso = [device newComputePipelineStateWithDescriptor:computeDesc options:0 reflection:&ref error:&err];
        //id<MTLComputePipelineState> cso = [device newComputePipelineStateWithFunction:computeFunc error:&err];
        [cso retain];
        item.cso = AS_VOID(cso);
        
        [lib release];
        [computeFunc release];
        		
        return m_shaders.AddItem(item);

    }
    
    const auto& depthStencilDesc = shaderDesc.depthStencilDesc;

    
    MTLRenderPipelineDescriptor *pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDescriptor.inputPrimitiveTopology = GetMTLTopology(shaderDesc.topology);
    pipelineDescriptor.alphaToCoverageEnabled = shaderDesc.alphaToCoverage;
    // pipelineDescriptor.supportIndirectCommandBuffers = YES;
    
    // Stencil
    MTLStencilDescriptor* backStencil = [[MTLStencilDescriptor alloc] init];
    MTLStencilDescriptor* frontStencil = [[MTLStencilDescriptor alloc] init];
    backStencil.depthFailureOperation = GetMTLStencilOperation(depthStencilDesc.backStencilState.depthFailOp);
    backStencil.depthStencilPassOperation = GetMTLStencilOperation(depthStencilDesc.backStencilState.passOp);
    backStencil.stencilCompareFunction = GetMTLCompareOp(depthStencilDesc.backStencilState.compareOp);
    backStencil.stencilFailureOperation = GetMTLStencilOperation(depthStencilDesc.backStencilState.failOp);
    backStencil.readMask = depthStencilDesc.stencilCompareMask;
    backStencil.writeMask = depthStencilDesc.stencilWriteMask;
    
    frontStencil.depthFailureOperation = GetMTLStencilOperation(depthStencilDesc.frontStencilState.depthFailOp);
    frontStencil.depthStencilPassOperation = GetMTLStencilOperation(depthStencilDesc.frontStencilState.passOp);
    frontStencil.stencilCompareFunction = GetMTLCompareOp(depthStencilDesc.frontStencilState.compareOp);
    frontStencil.stencilFailureOperation = GetMTLStencilOperation(depthStencilDesc.frontStencilState.failOp);
    frontStencil.readMask = depthStencilDesc.stencilCompareMask;
    frontStencil.writeMask = depthStencilDesc.stencilWriteMask;
    
    pipelineDescriptor.stencilAttachmentPixelFormat = depthStencilDesc.stencilEnabled ? GetMTLFormat(depthStencilDesc.depthStencilAttachmentFormat) : MTLPixelFormatInvalid;
    
    // Depth
    MTLDepthStencilDescriptor * depthStencilDescriptor = [[MTLDepthStencilDescriptor alloc] init];
    depthStencilDescriptor.depthWriteEnabled = depthStencilDesc.depthWrite;
    depthStencilDescriptor.depthCompareFunction = GetMTLCompareOp(depthStencilDesc.depthCompare);
    depthStencilDescriptor.frontFaceStencil = frontStencil;
    depthStencilDescriptor.backFaceStencil = backStencil;
    pipelineDescriptor.depthAttachmentPixelFormat = depthStencilDesc.depthWrite ? GetMTLFormat(depthStencilDesc.depthStencilAttachmentFormat) : MTLPixelFormatInvalid;

    const uint32 colorAttachments = static_cast<uint32>(shaderDesc.colorAttachments.size());
    for(uint32 i = 0; i < colorAttachments; i++)
    {
        const auto& att = shaderDesc.colorAttachments[i];
        const auto& blend = att.blendAttachment;
        pipelineDescriptor.colorAttachments[i].writeMask = 0;
        for(auto flag : blend.componentFlags)
        pipelineDescriptor.colorAttachments[i].writeMask |= GetMTLColorWriteMask(flag);
        pipelineDescriptor.colorAttachments[i].pixelFormat = GetMTLFormat(att.format);
        pipelineDescriptor.colorAttachments[i].blendingEnabled = blend.blendEnabled;
        pipelineDescriptor.colorAttachments[i].rgbBlendOperation = GetMTLBlendOp(blend.colorBlendOp);
        pipelineDescriptor.colorAttachments[i].alphaBlendOperation = GetMTLBlendOp(blend.alphaBlendOp);
        pipelineDescriptor.colorAttachments[i].sourceRGBBlendFactor= GetMTLBlendFactor(blend.srcColorBlendFactor);
        pipelineDescriptor.colorAttachments[i].destinationRGBBlendFactor = GetMTLBlendFactor(blend.dstColorBlendFactor);
        pipelineDescriptor.colorAttachments[i].sourceAlphaBlendFactor = GetMTLBlendFactor(blend.srcAlphaBlendFactor);
        pipelineDescriptor.colorAttachments[i].destinationAlphaBlendFactor= GetMTLBlendFactor(blend.dstAlphaBlendFactor);
    }
        
    
    MTLVertexDescriptor* vertexDescriptor = [[MTLVertexDescriptor alloc] init];
    
    uint32 i = 0;
    size_t totalSize = 0;
    for (const auto& input : shaderDesc.layout.vertexInputs)
    {
        vertexDescriptor.attributes[i].format = GetMTLVertexFormat(input.format);
        vertexDescriptor.attributes[i].offset = input.offset;
        vertexDescriptor.attributes[i].bufferIndex = 0;
        totalSize += input.size;
        i++;
    }
    
    if(!shaderDesc.layout.vertexInputs.empty())
    {
        vertexDescriptor.layouts[0].stride = totalSize;
        vertexDescriptor.layouts[0].stepRate = 1;
        vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
        [pipelineDescriptor setVertexDescriptor:vertexDescriptor];
    }
  

    // Stages
    LINAGX_MAP<ShaderStage, id<MTLLibrary>> libs;
    
    for(const auto& [stg, blob] : shaderDesc.stages)
    {
        void* data = LINAGX_MALLOC(blob.size);
        LINAGX_MEMCPY(data, blob.ptr, blob.size);
        std::string readData ((char*)data, blob.size);
        LINAGX_FREE(data);
        
        NSString* src = [NSString stringWithUTF8String:readData.c_str()];
        NSError* error = nil;
        libs[stg] = [device newLibraryWithSource:src options:nil error:&error];
        [libs[stg] retain];
        
        if(error != nil)
        {
            const char* errStr = [error.localizedDescription UTF8String];
            LOGE("Backend -> Shader compile error! %s", errStr);
        }

        NSString* entryPoint = [NSString stringWithUTF8String:shaderDesc.layout.entryPoints.at(stg).c_str()];
        
        id<MTLFunction> f = [libs[stg] newFunctionWithName:entryPoint];
        
        if(stg == ShaderStage::Compute)
        {
            LOGA(false, "!!");
        }
        else if(stg == ShaderStage::Fragment)
            pipelineDescriptor.fragmentFunction = f;
        else if(stg == ShaderStage::Vertex)
            pipelineDescriptor.vertexFunction = f;
        else{
            LOGA(false, "Unsupported stage!!");
        }
    }
    
    // Debug
    NAME_OBJ_CSTR(pipelineDescriptor, shaderDesc.debugName);
    
    NSError *error = nil;
    id<MTLRenderPipelineState> pso = [device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];
    [pso retain];
    item.pso = AS_VOID(pso);
    
    if(error != nil)
    {
        const char *cString = [error.description UTF8String];
        LOGE("Backend -> Error creating shader! %s", cString);
    }
    
    if(depthStencilDesc.depthWrite)
    {
        id<MTLDepthStencilState> dsso = [device newDepthStencilStateWithDescriptor:depthStencilDescriptor];
        [dsso retain];
        item.dsso = AS_VOID(dsso);
    }
    else
        item.dsso = nullptr;
    
    for(const auto& [stg, blob] : shaderDesc.stages)
    {
        [libs[stg] release];
    }
    
    return m_shaders.AddItem(item);
}

void MTLBackend::DestroyShader(uint16 handle) {
    auto& shader = m_shaders.GetItemR(handle);
    if (!shader.isValid)
    {
        LOGE("Backend -> Shader to be destroyed is not valid!");
        return;
    }
    
    if(shader.isCompute)
    {
        id<MTLComputePipelineState> cso = AS_MTL(shader.cso, id<MTLComputePipelineState>);
        [cso release];
    }
    else{
        id<MTLRenderPipelineState> pso = AS_MTL(shader.pso, id<MTLRenderPipelineState>);
        id<MTLDepthStencilState> dsso = AS_MTL(shader.dsso, id<MTLDepthStencilState>);
        [pso release];
        [dsso release];
    }

    m_shaders.RemoveItem(handle);
}

uint32 MTLBackend::CreateTexture(const TextureDesc &desc) {
    MTLTexture2D item = {};
    item.isValid = true;
    
    auto device = AS_MTL(m_device, id<MTLDevice>);
    
    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:GetMTLFormat(desc.format) width:desc.width height:desc.height mipmapped:desc.mipLevels > 1];
    textureDescriptor.mipmapLevelCount = desc.mipLevels;
    textureDescriptor.textureType = desc.arrayLength > 1 ? MTLTextureType2DArray : MTLTextureType2D;
    textureDescriptor.storageMode =  MTLStorageModePrivate;
    textureDescriptor.arrayLength = desc.arrayLength > 1 ? desc.arrayLength : 1;
    
    if(desc.usage == TextureUsage::ColorTexture || desc.usage == TextureUsage::ColorTextureDynamic)
        textureDescriptor.usage = MTLTextureUsageShaderRead;
    else if(desc.usage == TextureUsage::ColorTextureRenderTarget)
        textureDescriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
    else if(desc.usage == TextureUsage::DepthStencilTexture)
        textureDescriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
    
    
        
    id<MTLTexture> texture = [device newTextureWithDescriptor:textureDescriptor];
    [texture retain];
    item.ptr = AS_VOID(texture);
    NAME_OBJ_CSTR(texture, desc.debugName);
    
    return m_textures.AddItem(item);
}

void MTLBackend::DestroyTexture(uint32 handle) {
    auto& txt = m_textures.GetItemR(handle);
    if (!txt.isValid)
    {
        LOGE("Backend -> Texture to be destroyed is not valid!");
        return;
    }
    
    id<MTLTexture> texture = AS_MTL(txt.ptr, id<MTLTexture>);
    [texture release];
    
    m_textures.RemoveItem(handle);
}

uint32 MTLBackend::CreateSampler(const SamplerDesc &desc) {
    MTLSampler item ={};
    item.isValid = true;
    
    auto device = AS_MTL(m_device, id<MTLDevice>);
    MTLSamplerDescriptor *samplerDesc = [[MTLSamplerDescriptor alloc] init];
    samplerDesc.mipFilter =GetMTLMipFilter(desc.mipmapMode);
    samplerDesc.minFilter = GetMTLMinMagFilter(desc.minFilter);
    samplerDesc.magFilter = GetMTLMinMagFilter(desc.magFilter);
    samplerDesc.rAddressMode = samplerDesc.sAddressMode = samplerDesc.tAddressMode = GetMTLSamplerAddressMode(desc.mode);
    samplerDesc.lodMinClamp = desc.minLod;
    samplerDesc.maxAnisotropy = desc.anisotropy;
    samplerDesc.lodMaxClamp = desc.maxLod;
    samplerDesc.supportArgumentBuffers = true;
    NAME_OBJ_CSTR(samplerDesc, desc.debugName);

    id<MTLSamplerState> sampler = [device newSamplerStateWithDescriptor:samplerDesc];
    [sampler retain];
    item.ptr = AS_VOID(sampler);


    return m_samplers.AddItem(item);
}

void MTLBackend::DestroySampler(uint32 handle) {
    auto& item = m_samplers.GetItemR(handle);
    if (!item.isValid)
    {
        LOGE("Backend -> Sampler to be destroyed is not valid!");
        return;
    }
    
    id<MTLSamplerState> sampler = AS_MTL(item.ptr, id<MTLSamplerState>);
    [sampler release];
    
    m_samplers.RemoveItem(handle);
}

uint32 MTLBackend::CreateResource(const ResourceDesc &desc) {
    MTLResource item = {};
    item.isValid = true;
    item.heapType = desc.heapType;
    item.size = desc.size;
    
    auto device = AS_MTL(m_device, id<MTLDevice>);
    
    MTLResourceOptions options = 0;
   
    if(desc.heapType == ResourceHeap::GPUOnly)
        options |= MTLResourceStorageModePrivate;
    else if(desc.heapType == ResourceHeap::StagingHeap)
        options |= MTLStorageModeShared;
    else if(desc.heapType == ResourceHeap::CPUVisibleGPUMemory)
        options |= MTLStorageModeShared;
    
    id<MTLBuffer> buffer = [device newBufferWithLength:desc.size options:options];
    [buffer retain];
    item.ptr = AS_VOID(buffer);
    NAME_OBJ_CSTR(buffer, desc.debugName);

    return m_resources.AddItem(item);
}

void MTLBackend::MapResource(uint32 handle, uint8 *&ptr) {
    auto& item = m_resources.GetItemR(handle);
    
    if(item.heapType == ResourceHeap::GPUOnly)
    {
        LOGE("Backend -> Can not map gpu only resources!");
        return;
    }
    
    id<MTLBuffer> buffer = AS_MTL(item.ptr, id<MTLBuffer>);
    ptr = static_cast<uint8*>([buffer contents]);
}

void MTLBackend::UnmapResource(uint32 handle) {
    auto& item = m_resources.GetItemR(handle);
}

void MTLBackend::DestroyResource(uint32 handle) {
    auto& res = m_resources.GetItemR(handle);
    if (!res.isValid)
    {
        LOGE("Backend -> Resource to be destroyed is not valid!");
        return;
    }
   
    id<MTLBuffer> buffer = AS_MTL(res.ptr, id<MTLBuffer>);
    [buffer release];
    
    m_resources.RemoveItem(handle);
}

uint16 MTLBackend::CreateDescriptorSet(const DescriptorSetDesc &desc) {
    MTLDescriptorSet item = {};
    item.isValid = true;
    item.desc = desc;
  
    uint32 bufferID = 0, textureID = 0, samplerID = 0;
    auto device = AS_MTL(m_device, id<MTLDevice>);

    auto createArgEncoder = [device](MTLBinding& bnd, bool isSecondary, DescriptorType type) {
        NSMutableArray<MTLArgumentDescriptor *> *argDescriptors = [[NSMutableArray alloc] init];
        MTLArgumentDescriptor *bufferDescriptor = [[MTLArgumentDescriptor alloc] init];
        
        if(type == DescriptorType::SeparateImage)
            bufferDescriptor.dataType = MTLDataTypeTexture;
        else if(type == DescriptorType::SeparateSampler)
            bufferDescriptor.dataType = MTLDataTypeSampler;
        else if(type == DescriptorType::CombinedImageSampler)
            bufferDescriptor.dataType = isSecondary ? MTLDataTypeSampler : MTLDataTypeTexture;
        else
            bufferDescriptor.dataType = MTLDataTypeStruct;
        
        bufferDescriptor.index = 0;
        bufferDescriptor.access = MTLArgumentAccessReadWrite;
        [argDescriptors addObject:bufferDescriptor];
        id<MTLArgumentEncoder> argEncoder = [device newArgumentEncoderWithArguments:argDescriptors];
        [argEncoder retain];
        
        if(isSecondary)
            bnd.argEncoderSecondary = AS_VOID(argEncoder);
        else
            bnd.argEncoder = AS_VOID(argEncoder);
    };
    
    for(const auto& binding : desc.bindings)
    {
        MTLBinding bnd = {};
        bnd.lgxBinding = binding;
        
        if(binding.type == DescriptorType::UBO || binding.type == DescriptorType::SSBO)
        {
            bnd.localBufferID = bufferID;
            bufferID += binding.descriptorCount;
        }
        else if(binding.type == DescriptorType::CombinedImageSampler)
        {
            if(binding.unbounded)
            {
                bnd.localBufferID = bufferID;
                bufferID++;
                createArgEncoder(bnd, false, binding.type);

                bnd.localBufferIDSecondary = bufferID;
                bufferID++;
                createArgEncoder(bnd, true, binding.type);
            }
            else
            {
                bnd.localBufferID = textureID;
                textureID += binding.descriptorCount;
                
                bnd.localBufferIDSecondary = samplerID;
                samplerID += binding.descriptorCount;
            }
        }
        else if(binding.type == DescriptorType::SeparateImage)
        {
            if(binding.unbounded)
            {
                bnd.localBufferID = bufferID;
                bufferID++;
                createArgEncoder(bnd, false, binding.type);
            }
            else{
                bnd.localBufferID = textureID;
                textureID += binding.descriptorCount;
            }
        }
        else if(binding.type == DescriptorType::SeparateSampler)
        {
            if(binding.unbounded)
            {
                bnd.localBufferID = bufferID;
                bufferID++;
                createArgEncoder(bnd, false, binding.type);
            }
            else{
                bnd.localBufferID = samplerID;
                samplerID += binding.descriptorCount;
            }
        }
        
        item.bindings.push_back(bnd);

    }
    
    item.localBufferID = bufferID;
    item.localTextureID = textureID;
    item.localSamplerID = samplerID;
    
    return m_descriptorSets.AddItem(item);
}

void MTLBackend::DestroyDescriptorSet(uint16 handle) {
    auto& item = m_descriptorSets.GetItemR(handle);
    if (!item.isValid)
    {
        LOGE("Backend -> Descriptor set to be destroyed is not valid!");
        return;
    }
    
    for(const auto& bindingData : item.bindings)
    {
        if(bindingData.argBuffer != nullptr)
        {
            id<MTLBuffer> argBuffer = AS_MTL(bindingData.argBuffer, id<MTLBuffer>);
            [argBuffer release];
        }
        
        if(bindingData.argBufferSecondary != nullptr)
        {
            id<MTLBuffer> argBuffer = AS_MTL(bindingData.argBufferSecondary, id<MTLBuffer>);
            [argBuffer release];
        }
            
        if(bindingData.argEncoder != nullptr)
        {
            id<MTLArgumentEncoder> argEncoder = AS_MTL(bindingData.argEncoder, id<MTLArgumentEncoder>);
            [argEncoder release];
        }
        
        if(bindingData.argEncoderSecondary != nullptr)
        {
            id<MTLArgumentEncoder> argEncoder = AS_MTL(bindingData.argEncoderSecondary, id<MTLArgumentEncoder>);
            [argEncoder release];
        }
    }

    m_descriptorSets.RemoveItem(handle);
}

void MTLBackend::DescriptorUpdateBuffer(const DescriptorUpdateBufferDesc &desc) {
    
    auto& item = m_descriptorSets.GetItemR(desc.setHandle);
    auto device = AS_MTL(m_device, id<MTLDevice>);
    LOGA(desc.binding < static_cast<uint32>(item.bindings.size()), "Backend -> Binding is not valid!");

    auto& bindingData = item.bindings[desc.binding];
    const uint32 descriptorCount = static_cast<uint32>(desc.buffers.size());
    LOGA(descriptorCount <= bindingData.lgxBinding.descriptorCount, "Backend -> Error updating descriptor buffer as update count exceeds the maximum descriptor count for given binding!");
    LOGA(bindingData.lgxBinding.type == DescriptorType::UBO || bindingData.lgxBinding.type == DescriptorType::SSBO, "Backend -> You can only use DescriptorUpdateBuffer with descriptors of type UBO and SSBO! Use DescriptorUpdateImage()");
    
    bindingData.resources.clear();
    for(uint32 i = 0; i < descriptorCount; i++)
        bindingData.resources.push_back(desc.buffers[i]);
    
    if(bindingData.lgxBinding.unbounded)
    {
        LOGA(bindingData.lgxBinding.type != DescriptorType::SSBO, "Backend -> Can't use SSBO's as unbounded!");
        
        if(bindingData.argBuffer != nullptr)
        {
            id<MTLBuffer> argBuffer = AS_MTL(bindingData.argBuffer, id<MTLBuffer>);
            [argBuffer release];
        }
       
        id<MTLBuffer> argBuffer = [device newBufferWithLength:sizeof(id<MTLTexture>) * descriptorCount options:0];
        [argBuffer retain];
        bindingData.argBuffer = AS_VOID(argBuffer);
        
        id<MTLArgumentEncoder> encoder = AS_MTL(bindingData.argEncoder, id<MTLArgumentEncoder>);
        
        for(uint32 i = 0; i < descriptorCount; i++)
        {
            id<MTLBuffer> buf = AS_MTL(m_resources.GetItemR(bindingData.resources[i]).ptr, id<MTLBuffer>);
            [encoder setArgumentBuffer:argBuffer offset:encoder.encodedLength * i];
            [encoder setBuffer:buf offset:0 atIndex:0];
        }
    }
}

void MTLBackend::DescriptorUpdateImage(const DescriptorUpdateImageDesc &desc) {
    
    auto& item = m_descriptorSets.GetItemR(desc.setHandle);
    auto device = AS_MTL(m_device, id<MTLDevice>);
    LOGA(desc.binding < static_cast<uint32>(item.bindings.size()), "Backend -> Binding is not valid!");

    auto& bindingData = item.bindings[desc.binding];
    const uint32 txtDescriptorCount = static_cast<uint32>(desc.textures.size());
    const uint32 smpDescriptorCount = static_cast<uint32>(desc.samplers.size());
    LOGA(txtDescriptorCount <= bindingData.lgxBinding.descriptorCount && smpDescriptorCount <= bindingData.lgxBinding.descriptorCount, "Backend -> Error updateing descriptor buffer as update count exceeds the maximum descriptor count for given binding!");

    LOGA(bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler || bindingData.lgxBinding.type == DescriptorType::SeparateSampler || bindingData.lgxBinding.type == DescriptorType::SeparateImage, "Backend -> You can only use DescriptorUpdateImage with descriptors of type combined image sampler, separate image or separate sampler! Use DescriptorUpdateBuffer()");
    
    bindingData.resources.clear();
    bindingData.additionalResources.clear();
    uint32 usedDescriptorCount = 0;
    if(bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler)
    {
        LOGA(txtDescriptorCount == smpDescriptorCount, "Backend -> Trying to update combined image samplers but amount of texture and sampler resources are not the same!");
        usedDescriptorCount = txtDescriptorCount;
        for(uint32 i = 0; i < txtDescriptorCount; i++)
        {
            bindingData.resources.push_back(desc.textures[i]);
            bindingData.additionalResources.push_back(desc.samplers[i]);
        }
    }
    else if(bindingData.lgxBinding.type == DescriptorType::SeparateSampler)
    {
        usedDescriptorCount = smpDescriptorCount;
        for(uint32 i = 0; i < smpDescriptorCount; i++)
            bindingData.resources.push_back(desc.samplers[i]);
    }
    else if(bindingData.lgxBinding.type == DescriptorType::SeparateImage)
    {
        usedDescriptorCount = txtDescriptorCount;
        for(uint32 i = 0; i < txtDescriptorCount; i++)
            bindingData.resources.push_back(desc.textures[i]);
    }
    
     
    if(bindingData.lgxBinding.unbounded)
    {
        auto processForTexture = [&](){
            
            id<MTLBuffer> argBuffer = [device newBufferWithLength:sizeof(id<MTLTexture>) * usedDescriptorCount options:0];
            [argBuffer retain];
            bindingData.argBuffer = AS_VOID(argBuffer);

            id<MTLArgumentEncoder> encoder = AS_MTL(bindingData.argEncoder, id<MTLArgumentEncoder>);
            auto hm = encoder.encodedLength;
            for(uint32 i = 0; i < usedDescriptorCount; i++)
            {
                id<MTLTexture> txt = AS_MTL(m_textures.GetItemR(bindingData.resources[i]).ptr, id<MTLTexture>);
                [encoder setArgumentBuffer:argBuffer offset:encoder.encodedLength * i];
                [encoder setTexture:txt atIndex:0];
            }
        };
        
        auto processForSampler = [&](bool useSecondary){
            
            id<MTLBuffer> argBuffer = [device newBufferWithLength:sizeof(id<MTLSamplerState>) * usedDescriptorCount options:0];
            [argBuffer retain];
            
            if(useSecondary)
                bindingData.argBufferSecondary = AS_VOID(argBuffer);
            else
                bindingData.argBuffer = AS_VOID(argBuffer);

            id<MTLArgumentEncoder> encoder = AS_MTL(useSecondary ? bindingData.argEncoderSecondary : bindingData.argEncoder, id<MTLArgumentEncoder>);
            for(uint32 i = 0; i < usedDescriptorCount; i++)
            {
                const uint32 handle = useSecondary ? bindingData.additionalResources[i] : bindingData.resources[i];
                id<MTLSamplerState> smp = AS_MTL(m_samplers.GetItemR(handle).ptr, id<MTLSamplerState>);
                [encoder setArgumentBuffer:argBuffer offset:encoder.encodedLength * i];
                [encoder setSamplerState:smp atIndex:0];
            }
        };
        
        if(bindingData.argBuffer != nullptr)
        {
            id<MTLBuffer> argBuffer = AS_MTL(bindingData.argBuffer, id<MTLBuffer>);
            [argBuffer release];
        }
        
        if(bindingData.argBufferSecondary != nullptr)
        {
            id<MTLBuffer> argBuffer = AS_MTL(bindingData.argBufferSecondary, id<MTLBuffer>);
            [argBuffer release];
        }
        
        if(bindingData.lgxBinding.type == DescriptorType::CombinedImageSampler)
        {
            processForTexture();
            processForSampler(true);
        }
        else if(bindingData.lgxBinding.type == DescriptorType::SeparateImage)
            processForTexture();
        else
            processForSampler(false);
       
    }
    
}

uint16 MTLBackend::CreatePipelineLayout(const LinaGX::PipelineLayoutDesc &desc) {
    MTLPipelineLayout item = {};
    item.isValid = true;
    
    return m_pipelineLayouts.AddItem(item);
}

void MTLBackend::DestroyPipelineLayout(uint16 handle)
{
    auto& lyt = m_cmdStreams.GetItemR(handle);
    if (!lyt.isValid)
    {
        LOGE("Backend -> Pipeline Layout to be destroyed is not valid!");
        return;
    }
    
    m_pipelineLayouts.RemoveItem(handle);
}


uint32 MTLBackend::CreateCommandStream(CommandType cmdType) {
    MTLCommandStream item = {};
    item.isValid = true;
    item.type = cmdType;
    
    return m_cmdStreams.AddItem(item);
}

void MTLBackend::DestroyCommandStream(uint32 handle) {
    auto& stream = m_cmdStreams.GetItemR(handle);
    if (!stream.isValid)
    {
        LOGE("Backend -> Command Stream to be destroyed is not valid!");
        return;
    }
    
    for (const auto [id, frame] : stream.intermediateResources)
        DestroyResource(id);
    
    if(stream.indirectCommandBuffer != nullptr)
    {
        id<MTLIndirectCommandBuffer> buf = AS_MTL(stream.indirectCommandBuffer, id<MTLIndirectCommandBuffer>);
        [buf release];
        stream.indirectCommandBuffer = nullptr;
    }
    
    stream.intermediateResources.clear();

    m_cmdStreams.RemoveItem(handle);
}

void MTLBackend::CloseCommandStreams(CommandStream **streams, uint32 streamCount) {
    
    const auto& q = m_queues.GetItemR(GetPrimaryQueue(CommandType::Graphics));
    id<MTLCommandQueue> queue = AS_MTL(q.queue, id<MTLCommandQueue>);
    
    for (uint32 i = 0; i < streamCount; i++)
    {
        auto  stream    = streams[i];
        auto& sr        = m_cmdStreams.GetItemR(stream->m_gpuHandle);

        if(sr.type == CommandType::Secondary)
            return;

        if (stream->m_commandCount == 0)
            continue;

        id<MTLCommandBuffer> buffer = [queue commandBuffer];
        [buffer retain];
        sr.currentBuffer = AS_VOID(buffer);
    
        auto endCurrentComputeEncoder = [&](){
            id<MTLComputeCommandEncoder> computeEncoder = AS_MTL(sr.currentComputeEncoder, id<MTLComputeCommandEncoder>);
            [computeEncoder endEncoding];
            sr.currentComputeEncoder = nullptr;
        };
        
        auto endCurrentBlitEncoder = [&] () {
            id<MTLBlitCommandEncoder> blitEncoder = AS_MTL(sr.currentBlitEncoder, id<MTLBlitCommandEncoder>);
            [blitEncoder endEncoding];
            sr.currentBlitEncoder = nullptr;
        };
        
         for (uint32 i = 0; i < stream->m_commandCount; i++)
         {
             uint8*        data = stream->m_commands[i];
             LINAGX_TYPEID tid  = 0;
             LINAGX_MEMCPY(&tid, data, sizeof(LINAGX_TYPEID));
             const size_t increment = sizeof(LINAGX_TYPEID);
             uint8*       cmd       = data + increment;

             // Starting blit ops but no blit encoder.
             if(tid == LGX_GetTypeID<CMDCopyResource>() || tid == LGX_GetTypeID<CMDCopyBufferToTexture2D>())
             {
                 if(sr.currentBlitEncoder == nullptr)
                 {
                     id<MTLBlitCommandEncoder> blitEncoder = [buffer blitCommandEncoder];
                     [blitEncoder retain];
                     sr.currentBlitEncoder = blitEncoder;
                     sr.allBlitEncoders.push_back(sr.currentBlitEncoder);
                 }
             }
             else if(tid == LGX_GetTypeID<CMDBindPipeline>())
             {
                 // Binding pipeline, if blit encoder end it.
                 // If compute encoder, end if it binding non-compute pipeline.
                 // If no compute encoder, but binding compute pipeline, create it.
                 
                 CMDBindPipeline* pipeline = reinterpret_cast<CMDBindPipeline*>(cmd);
                 const auto& shader = m_shaders.GetItemR(pipeline->shader);
                 
                 if(sr.currentBlitEncoder != nullptr)
                     endCurrentBlitEncoder();
                 
                 if(!shader.isCompute && sr.currentComputeEncoder != nullptr)
                 {
                     endCurrentComputeEncoder();
                 }
                 else if(shader.isCompute && sr.currentComputeEncoder == nullptr)
                 {
                     id<MTLComputeCommandEncoder> computeEncoder = [buffer computeCommandEncoder];
                     [computeEncoder retain];
                     sr.currentComputeEncoder = computeEncoder;
                     sr.allComputeEncoders.push_back(sr.currentComputeEncoder);
                 }
             }
             else if(tid == LGX_GetTypeID<CMDBindDescriptorSets>() || tid == LGX_GetTypeID<CMDBindConstants>())
             {
                 // computer encoder exists, binding resources on non-compute shader(shouldn't be possible tbh)
                 // end blit if exists & compute.
                 
                 if(sr.currentBlitEncoder != nullptr)
                     endCurrentBlitEncoder();
                 
                 if(sr.currentComputeEncoder && !sr.currentShaderIsCompute)
                     endCurrentComputeEncoder();
             }
             else if(tid == LGX_GetTypeID<CMDDispatch>())
             {
                 if(sr.currentComputeEncoder == nullptr)
                 {
                     id<MTLComputeCommandEncoder> computeEncoder = [buffer computeCommandEncoder];
                     [computeEncoder retain];
                     sr.currentComputeEncoder = computeEncoder;
                     sr.allComputeEncoders.push_back(sr.currentComputeEncoder);
                 }
             }
             else{
              
                 if(sr.currentBlitEncoder != nullptr)
                     endCurrentBlitEncoder();
                 
                 if(sr.currentComputeEncoder != nullptr)
                     endCurrentComputeEncoder();
             }
             
             // Include as a part of this one.
             if(tid == LGX_GetTypeID<CMDExecuteSecondaryStream>())
             {
                 CMDExecuteSecondaryStream* exec = reinterpret_cast<CMDExecuteSecondaryStream*>(cmd);
                 auto* secondaryStream = exec->secondaryStream;
                 
                 for(uint32 j = 0; j < secondaryStream->m_commandCount; j++)
                 {
                     uint8*        dataSecondary = secondaryStream->m_commands[i];
                     LINAGX_TYPEID secondaryTid  = 0;
                     LINAGX_MEMCPY(&secondaryTid, dataSecondary, sizeof(LINAGX_TYPEID));
                     const size_t incrementSecondary = sizeof(LINAGX_TYPEID);
                     uint8*       cmdSecondary       = dataSecondary + incrementSecondary;
                     (this->*m_cmdFunctions[tid])(cmdSecondary, sr);
                 }
             }
             
             (this->*m_cmdFunctions[tid])(cmd, sr);
         }
        
        if(sr.currentBlitEncoder)
            endCurrentBlitEncoder();
        
        if(sr.currentComputeEncoder != nullptr)
            endCurrentComputeEncoder();
        
        sr.boundDescriptorSets.clear();
    }
}

void MTLBackend::SubmitCommandStreams(const SubmitDesc &desc) {
          
    auto& pfd = m_perFrameData[m_currentFrameIndex];
    
    if (desc.isMultithreaded)
    {
        // spinlock
        while (m_submissionFlag.test_and_set(std::memory_order_acquire))
        {
            
        }
    }
    
    const auto& queue = m_queues.GetItemR(desc.targetQueue);
    
    if(queue.type == CommandType::Graphics)
        pfd.submits++;
    
    if(desc.isMultithreaded)
        m_submissionFlag.clear();
  
    for (uint32 i = 0; i < desc.streamCount; i++)
    {
        auto stream = desc.streams[i];
        if (stream->m_commandCount == 0)
        {
            LOGE("Backend -> Can not execute stream as no commands were recorded!");
            continue;
        }

        auto& str = m_cmdStreams.GetItemR(stream->m_gpuHandle);
        LOGA(str.type != CommandType::Secondary, "Backend -> Can not submit command streams of type Secondary directly to the queues! Use CMDExecuteSecondary instead!");
        id<MTLCommandBuffer> buffer = AS_MTL(str.currentBuffer, id<MTLCommandBuffer>);
        
        if(desc.useWait)
        {
            for (uint32 j = 0; j < desc.waitCount; j++)
            {
                const auto& us = m_userSemaphores.GetItemR(desc.waitSemaphores[j]);
                id<MTLSharedEvent> ev = AS_MTL(us.semaphore, id<MTLSharedEvent>);
                [buffer encodeWaitForEvent:ev value:desc.waitValues[j]];
            }
        }
        
        if(queue.type == CommandType::Graphics)
        {
            [buffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
              
                while (m_submissionFlag.test_and_set(std::memory_order_acquire))
                {
                }
                pfd.reachedSubmits++;
                m_submissionFlag.clear();
            }];
            
        }
    
        if(desc.useSignal)
        {
            for (uint32 j = 0; j < desc.signalCount; j++)
            {
                const auto& us = m_userSemaphores.GetItemR(desc.signalSemaphores[j]);
                id<MTLSharedEvent> ev = AS_MTL(us.semaphore, id<MTLSharedEvent>);
                [buffer encodeSignalEvent:ev value:desc.signalValues[j]];
            }
        }
        
        for(auto swp : str.writtenSwapchains)
        {
            const auto& swap = m_swapchains.GetItemR(swp);
            id<CAMetalDrawable> drawable = AS_MTL(swap._currentDrawable, id<CAMetalDrawable>);
            [buffer presentDrawable:drawable];
        }
        
        [buffer commit];
        [buffer release];
        
        for(auto ptr : str.allBlitEncoders)
        {
            id<MTLBlitCommandEncoder> encoder = AS_MTL(ptr, id<MTLBlitCommandEncoder>);
            [encoder release];
        }
        
        for(auto ptr : str.allRenderEncoders)
        {
            id<MTLRenderCommandEncoder> encoder = AS_MTL(ptr, id<MTLRenderCommandEncoder>);
            [encoder release];
        }
        
        for(auto ptr : str.allComputeEncoders)
        {
            id<MTLComputeCommandEncoder> encoder = AS_MTL(ptr, id<MTLComputeCommandEncoder>);
            [encoder release];
        }
        
        str.currentShader = 0;
        str.currentEncoder =  str.currentBlitEncoder = str.currentComputeEncoder = str.currentBuffer = nullptr;
        str.containsDelayedVtxBind = false;
        str.currentRenderPassUseDepth = false;
        str.currentEncoderDepthStencil = nullptr;
        str.currentShaderExists = false;
        str.writtenSwapchains.clear();
        str.allBlitEncoders.clear();
        str.allRenderEncoders.clear();
        str.allComputeEncoders.clear();
        
        m_submissionPerFrame.store(m_submissionPerFrame + 1);
    }
}

uint8 MTLBackend::CreateQueue(const QueueDesc &desc) {
    MTLQueue item = {};
    item.isValid = true;
    item.type = desc.type;
    
    auto device = AS_MTL(m_device, id<MTLDevice>);
    
    if(m_queues.GetNextFreeID() == 0)
    {
        id<MTLCommandQueue> q = [device newCommandQueue];
        item.queue = AS_VOID(q);
        NAME_OBJ_CSTR(q, desc.debugName);
        [q retain];
    }
    else{
        item.queue = m_queues.GetItemR(0).queue;
    }
    
    return m_queues.AddItem(item);
}

void MTLBackend::DestroyQueue(uint8 queue) {
    auto& item = m_queues.GetItemR(queue);
    if (!item.isValid)
    {
        LOGE("Backend -> Queue to be destroyed is not valid!");
        return;
    }
    
    if(queue == 0)
    {
        id<MTLCommandQueue> q = AS_MTL(item.queue, id<MTLCommandQueue>);
        [q release];
    }
    
    m_queues.RemoveItem(queue);
}

uint8 MTLBackend::GetPrimaryQueue(CommandType type) {
    LOGA(type != CommandType::Secondary, "Backend -> No queues of type Secondary exists, use either Graphics, Transfer or Compute!");
    return m_primaryQueues[static_cast<uint32>(type)];
}


bool MTLBackend::Initialize(const InitInfo &initInfo) {
        
    m_initInfo = initInfo;
    
    NSString *currentDirectory = [[NSFileManager defaultManager] currentDirectoryPath];
        NSLog(@"Current directory: %@", currentDirectory);
    
    NSArray<id<MTLDevice>> *availableDevices = MTLCopyAllDevices();
    id<MTLDevice> selectedDevice = nil;

    for (id<MTLDevice> device in availableDevices) {
        if (m_initInfo.gpu == PreferredGPUType::Discrete && [device isLowPower]) {
            continue;
        }
        if (m_initInfo.gpu == PreferredGPUType::Integrated && ![device isLowPower]) {
            continue;
        }
        selectedDevice = device;
        break;  // Found a device that meets our criteria
    }
    
    if(selectedDevice == nil)
        selectedDevice = MTLCreateSystemDefaultDevice();
    
    [selectedDevice retain];
    m_device = AS_VOID(selectedDevice);
    
    id<MTLDevice> target = AS_MTL(m_device, id<MTLDevice>);
    
    if (!m_device) {
        LOGE("Backend -> Failed creating system default device!");
        return false;
    }
    
    // Primary queues.
    {
        QueueDesc desc;
        desc.type                         = CommandType::Graphics;
        desc.debugName                    = "Primary Queue";
        m_primaryQueues[0] = CreateQueue(desc);
        desc.type = CommandType::Transfer;
        m_primaryQueues[1] = CreateQueue(desc);
        desc.type = CommandType::Compute;
        m_primaryQueues[2] = CreateQueue(desc);
    }
    
    // Per frame
    {
        for (uint32 i = 0; i < initInfo.framesInFlight; i++)
        {
            MTLPerFrameData pfd = {};
            m_perFrameData.push_back(pfd);
        }
    }
    
    // Command functions
    {
        BACKEND_BIND_COMMANDS(MTLBackend);
    }
    
    // TODO: CPU visible GPU memory stuff.
    
    auto device = AS_MTL(m_device, id<MTLDevice>);
    GPUInfo.totalCPUVisibleGPUMemorySize = device.hasUnifiedMemory ? 1 : 0;
    
    // TODO: pixel format support?
    // const uint32 last = static_cast<uint32>(Format::FORMAT_MAX);
    // for (uint32 i = 0; i < last; i++)
    // {
    //     const Format lgxFormat = static_cast<Format>(i);
    //     MTLPixelFormat pf = GetMTLFormat(lgxFormat);
    //
    // }
    //
    // for (auto& f : m_initInfo.checkForFormatSupport)
    // {
    //     auto it = std::find_if(GPUInfo.supportedImageFormats.begin(), GPUInfo.supportedImageFormats.end(), [&](Format format) { return f == format; });
    //     if (it == GPUInfo.supportedImageFormats.end())
    //         LOGE("Backend -> Required format is not supported by the GPU device! %d", static_cast<int>(f));
    // }
    
    
    return true;
}

void MTLBackend::Shutdown() {
            
    for (uint32 i = 0; i < m_initInfo.framesInFlight; i++)
    {
        MTLPerFrameData& pfd = m_perFrameData[i];
    }
    
    DestroyQueue(m_primaryQueues[0]);
    DestroyQueue(m_primaryQueues[1]);
    DestroyQueue(m_primaryQueues[2]);
    
    auto device = AS_MTL(m_device, id<MTLDevice>);
    [device release];
    
    for (auto& swp : m_swapchains)
    {
        LOGA(!swp.isValid, "Backend -> Some swapchains were not destroyed!");
    }

    for (auto& shader : m_shaders)
    {
        LOGA(!shader.isValid, "Backend -> Some shaders were not destroyed!");
    }

    for (auto& txt : m_textures)
    {
        LOGA(!txt.isValid, "Backend -> Some textures were not destroyed!");
    }

    for (auto& str : m_cmdStreams)
    {
        LOGA(!str.isValid, "Backend -> Some command streams were not destroyed!");
    }

    for (auto& r : m_resources)
    {
        LOGA(!r.isValid, "Backend -> Some resources were not destroyed!");
    }

    for (auto& r : m_userSemaphores)
    {
        LOGA(!r.isValid, "Backend ->Some semaphores were not destroyed!");
    }

    for (auto& r : m_samplers)
    {
        LOGA(!r.isValid, "Backend -> Some samplers were not destroyed!");
    }

    for (auto& r : m_descriptorSets)
    {
        LOGA(!r.isValid, "Backend -> Some descriptor sets were not destroyed!");
    }

    for (auto& q : m_queues)
    {
        LOGA(!q.isValid, "Backend -> Some queues were not destroyed!");
    }
    
    for (auto& l : m_pipelineLayouts)
    {
        LOGA(!l.isValid, "Backend -> Some pipeline layouts were not destroyed!");
    }
}

void MTLBackend::Join() {
        
    for(uint32 i = 0; i < m_initInfo.framesInFlight; i++)
    {
        const auto& pfd = m_perFrameData[i];
        
        // block.
        while(pfd.reachedSubmits < pfd.submits)
        {
            
        }
    }
}

void MTLBackend::StartFrame(uint32 frameIndex) {
        
    m_submissionPerFrame = 0;
    m_currentFrameIndex = frameIndex;
    auto& pfd = m_perFrameData[m_currentFrameIndex];
    
    m_frameOnGoing = true;
    
    // Block.
    while(pfd.reachedSubmits < pfd.submits)
    {
    }
    
    for(auto& swp : m_swapchains)
    {
        if(!swp.isValid || swp.width == 0 || swp.height == 0 || !swp.isActive)
            continue;
        
        swp._currentDrawableIndex = (swp._currentDrawableIndex + 1) % m_initInfo.backbufferCount;
        
        CAMetalLayer* layer = AS_MTL(swp.layer, CAMetalLayer*);
        id<CAMetalDrawable> drawable = [layer nextDrawable];
        [drawable retain];
        
        if(drawable)
            swp._currentDrawable = AS_VOID(drawable);
    }
    
    const uint32 next = m_cmdStreams.GetNextFreeID();
    for (uint32 i = 0; i < next; i++)
    {
        auto& cs = m_cmdStreams.GetItemR(i);

        if (!cs.isValid)
            continue;

        for (auto it = cs.intermediateResources.begin(); it != cs.intermediateResources.end();)
        {
            if (PerformanceStats.totalFrames > it->second + m_initInfo.framesInFlight + 1)
            {
                DestroyResource(it->first);
                it = cs.intermediateResources.erase(it);
            }
            else
                ++it;
        }
    }
}

void MTLBackend::Present(const PresentDesc &present) {
            
}

void MTLBackend::EndFrame() {
    m_frameOnGoing = false;
    LOGA((m_submissionPerFrame < m_initInfo.gpuLimits.maxSubmitsPerFrame), "Backend -> Exceeded maximum submissions per frame! Please increase the limit.");
}


void MTLBackend::BindDescriptorSets(MTLCommandStream &stream)
{
    if(stream.currentEncoder == nullptr && stream.currentComputeEncoder == nullptr)
        return;
    
    if(!stream.currentShaderExists)
        return;
    
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    id<MTLComputeCommandEncoder> computeEncoder = AS_MTL(stream.currentComputeEncoder, id<MTLComputeCommandEncoder>);
    
    #pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"

    auto setBuffer = [&](id<MTLBuffer> const* _Nonnull ptr, uint32 bufferIndex, uint32 count, ShaderStage stage, uint32 offset = 0){
       
        if(stage == ShaderStage::Vertex)
            bufferIndex = bufferIndex + 1;
        
       NSRange rng = NSMakeRange(bufferIndex, count);

       LINAGX_VEC<NSUInteger> offsets;
       offsets.resize(count);
        
       for(uint32 i = 0 ; i < count; i++)
            offsets[i] = offset;
       
        if(stage == ShaderStage::Fragment)
            [encoder setFragmentBuffers:ptr offsets:&offsets[0] withRange:rng];
        else if(stage == ShaderStage::Compute)
            [computeEncoder setBuffers:ptr offsets:&offsets[0] withRange:rng];
        else
            [encoder setVertexBuffers:ptr offsets:&offsets[0] withRange:rng];
    };
    
    auto setTexture = [&](id<MTLTexture> const* _Nonnull ptr, uint32 textureIndex, uint32 count, ShaderStage stage){
        NSRange rng = NSMakeRange(textureIndex, count);

       if(stage == ShaderStage::Compute)
            [computeEncoder setTextures:ptr withRange:rng];
       else if(stage == ShaderStage::Fragment)
           [encoder setFragmentTextures:ptr withRange:rng];
       else
           [encoder setVertexTextures:ptr withRange:rng];
    };
    
    auto setSampler = [&](id<MTLSamplerState> const* _Nonnull ptr, uint32 samplerIndex, uint32 count, ShaderStage stage){
        NSRange rng = NSMakeRange(samplerIndex, count);

        if(stage == ShaderStage::Compute)
            [computeEncoder setSamplerStates:ptr withRange:rng];
        else if(stage == ShaderStage::Fragment)
            [encoder setFragmentSamplerStates:ptr withRange:rng];
        else
            [encoder setVertexSamplerStates:ptr withRange:rng];
    };
    
#pragma clang diagnostic pop
    
    uint32 dynamicOffsetIndexCounter = 0;
    
    for(const auto& [setIndex, setData] : stream.boundSets)
    {
        if(!setData.isDirty)
            continue;
        
        auto& set = m_descriptorSets.GetItemR(setData.handle);
        
        LINAGX_VEC<ShaderStage> allStages;
        LINAGX_MAP<ShaderStage, uint32> bufferIDStart;
        LINAGX_MAP<ShaderStage, uint32> textureIDStart;
        LINAGX_MAP<ShaderStage, uint32> samplerIDStart;

        for(uint32 j = 0; j < setIndex; j++)
        {
            const auto& it = stream.boundSets.find(j);
            if(it == stream.boundSets.end())
                continue;
            
            const auto& boundSet = m_descriptorSets.GetItemR(it->second.handle);
            
            for(const auto& binding : boundSet.desc.bindings)
            {
                for(auto stg : binding.stages)
                {
                    bool found = false;
                    for(auto existingStg : allStages)
                    {
                        if(existingStg == stg)
                        {
                            found = true;
                            break;
                        }
                    }
                    
                    if(!found)
                        allStages.push_back(stg);
                }
            }
            
            for(auto stg: allStages)
            {
                bufferIDStart[stg] += boundSet.localBufferID;
                textureIDStart[stg] += boundSet.localTextureID;
                samplerIDStart[stg] += boundSet.localSamplerID;
            }
        }
        
        uint32 dynamicOffsetCounter = 0;
        
        for(const auto& mtlBinding : set.bindings)
        {
            for(auto stg : mtlBinding.lgxBinding.stages)
            {
                if(mtlBinding.lgxBinding.type == DescriptorType::CombinedImageSampler)
                {
                    LINAGX_VEC<id<MTLTexture>> textures;
                    LINAGX_VEC<id<MTLSamplerState>> samplers;
                    uint32 dcCount = static_cast<uint32>(mtlBinding.resources.size());
                    textures.resize(mtlBinding.resources.size());
                    samplers.resize(mtlBinding.resources.size());
                    
                    for(uint32 k = 0; k < dcCount; k++)
                    {
                        const auto& txt = m_textures.GetItemR(mtlBinding.resources[k]);
                        id<MTLTexture> mtlTexture = AS_MTL(txt.ptr, id<MTLTexture>);
                        textures[k] = mtlTexture;
                        
                        const auto& sampler = m_samplers.GetItemR(mtlBinding.additionalResources[k]);
                        id<MTLSamplerState> mtlSampler = AS_MTL(sampler.ptr, id<MTLSamplerState>);
                        samplers[k] = mtlSampler;
                    }
                   
                    // gonna be a buffer.
                    if(!mtlBinding.lgxBinding.unbounded)
                    {
                        setTexture(&textures[0], textureIDStart[stg] + mtlBinding.localBufferID, dcCount, stg);
                        setSampler(&samplers[0], samplerIDStart[stg] + mtlBinding.localBufferIDSecondary, dcCount, stg);
                    }
                    else
                    {
                        id<MTLBuffer> argBuffer = AS_MTL(mtlBinding.argBuffer, id<MTLBuffer>);
                        id<MTLBuffer> argBuffer2 = AS_MTL(mtlBinding.argBufferSecondary, id<MTLBuffer>);
                        setBuffer(&argBuffer, bufferIDStart[stg] + mtlBinding.localBufferID, 1, stg);
                        setBuffer(&argBuffer2, bufferIDStart[stg] + mtlBinding.localBufferIDSecondary, 1, stg);

                        [encoder useResources:&textures[0] count: dcCount usage:MTLResourceUsageRead stages:GetMTLRenderStage(stg)];
                    }
                   
                }
                else if(mtlBinding.lgxBinding.type == DescriptorType::SeparateImage)
                {
                    LINAGX_VEC<id<MTLTexture>> textures;
                    textures.resize(mtlBinding.resources.size());
                    uint32 dcCount = static_cast<uint32>(mtlBinding.resources.size());

                    for(uint32 k = 0; k < dcCount; k++)
                    {
                        const auto& txt = m_textures.GetItemR(mtlBinding.resources[k]);
                        id<MTLTexture> mtlTexture = AS_MTL(txt.ptr, id<MTLTexture>);
                        textures[k] = mtlTexture;
                    }
                    
                    if(!mtlBinding.lgxBinding.unbounded)
                        setTexture(&textures[0], textureIDStart[stg] + mtlBinding.localBufferID, dcCount, stg);
                    else
                    {
                        id<MTLBuffer> argBuffer = AS_MTL(mtlBinding.argBuffer, id<MTLBuffer>);
                        setBuffer(&argBuffer, bufferIDStart[stg] + mtlBinding.localBufferID, 1, stg);
                        [encoder useResources:&textures[0] count: dcCount usage:MTLResourceUsageRead stages:GetMTLRenderStage(stg)];
                    }
                    
                }
                else if(mtlBinding.lgxBinding.type == DescriptorType::SeparateSampler)
                {
                    LINAGX_VEC<id<MTLSamplerState>> samplers;
                    samplers.resize(mtlBinding.resources.size());
                    uint32 dcCount = static_cast<uint32>(mtlBinding.resources.size());

                    for(uint32 k = 0; k < dcCount; k++)
                    {
                        const auto& sampler = m_samplers.GetItemR(mtlBinding.resources[k]);
                        id<MTLSamplerState> mtlSampler = AS_MTL(sampler.ptr, id<MTLSamplerState>);
                        samplers[k] = mtlSampler;
                    }
                    
                    if(!mtlBinding.lgxBinding.unbounded)
                        setSampler(&samplers[0], samplerIDStart[stg] + mtlBinding.localBufferID, dcCount, stg);
                    else
                    {
                        id<MTLBuffer> argBuffer = AS_MTL(mtlBinding.argBuffer, id<MTLBuffer>);
                        setBuffer(&argBuffer, bufferIDStart[stg] + mtlBinding.localBufferID, 1, stg);
                    }
                }
                else if(mtlBinding.lgxBinding.type == DescriptorType::UBO)
                {
                    LINAGX_VEC<id<MTLBuffer>> ubos;
                    ubos.resize(mtlBinding.resources.size());
                    uint32 dcCount = static_cast<uint32>(mtlBinding.resources.size());

                    for(uint32  k = 0; k < dcCount; k++)
                    {
                        const auto& res = m_resources.GetItemR(mtlBinding.resources[k]);
                        id<MTLBuffer> mtlBuffer = AS_MTL(res.ptr, id<MTLBuffer>);
                        ubos[k] = mtlBuffer;
                    }
                    
                    uint32 offset = 0;
                    
                    if(mtlBinding.lgxBinding.useDynamicOffset)
                    {
                        offset = setData.dynamicOffsets[dynamicOffsetIndexCounter];
                        dynamicOffsetIndexCounter++;
                    }
                    
                    if(!mtlBinding.lgxBinding.unbounded)
                        setBuffer(&ubos[0], bufferIDStart[stg] + mtlBinding.localBufferID, dcCount, stg, offset);
                    else
                    {
                        id<MTLBuffer> argBuffer = AS_MTL(mtlBinding.argBuffer, id<MTLBuffer>);
                        setBuffer(&argBuffer, bufferIDStart[stg] + mtlBinding.localBufferID, 1, stg, offset);
                        [encoder useResources:&ubos[0] count: dcCount usage:MTLResourceUsageRead stages:GetMTLRenderStage(stg)];
                    }
                }
                else if(mtlBinding.lgxBinding.type == DescriptorType::SSBO)
                {
                    LINAGX_VEC<id<MTLBuffer>> ssbo;
                    ssbo.resize(mtlBinding.resources.size());
                    uint32 dcCount = static_cast<uint32>(mtlBinding.resources.size());

                    for(uint32  k = 0; k < dcCount; k++)
                    {
                        const auto& res = m_resources.GetItemR(mtlBinding.resources[k]);
                        id<MTLBuffer> mtlBuffer = AS_MTL(res.ptr, id<MTLBuffer>);
                        ssbo[k] = mtlBuffer;
                    }
                    
                    uint32 offset = 0;
                    
                    if(mtlBinding.lgxBinding.useDynamicOffset)
                    {
                        offset = setData.dynamicOffsets[dynamicOffsetIndexCounter];
                        dynamicOffsetIndexCounter++;
                    }
                    
                    if(!mtlBinding.lgxBinding.unbounded)
                        setBuffer(&ssbo[0], bufferIDStart[stg] + mtlBinding.localBufferID, dcCount, stg, offset);
                    else
                    {
                        LOGA(false, "Backend -> Can't use SSBOs as unbounded!");
                    }
                }
            }
        }
    }
}

void MTLBackend::CMD_BeginRenderPass(uint8 *data, MTLCommandStream &stream) {
    CMDBeginRenderPass* begin = reinterpret_cast<CMDBeginRenderPass*>(data);
    id<MTLCommandBuffer> buffer = AS_MTL(stream.currentBuffer, id<MTLCommandBuffer>);
    
    stream.currentRenderPassUseDepth = begin->depthStencilAttachment.useDepth;
    
    MTLRenderPassDescriptor* passDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    if(begin->depthStencilAttachment.useDepth)
    {
        passDescriptor.depthAttachment.loadAction = GetMTLLoadOp(begin->depthStencilAttachment.depthLoadOp);
        passDescriptor.depthAttachment.storeAction = GetMTLStoreOp(begin->depthStencilAttachment.depthStoreOp);
        passDescriptor.depthAttachment.clearDepth = begin->depthStencilAttachment.clearDepth;
        
        id<MTLTexture> depth = AS_MTL(m_textures.GetItemR(begin->depthStencilAttachment.texture).ptr, id<MTLTexture>);
        passDescriptor.depthAttachment.texture = depth;
    }

    if(begin->depthStencilAttachment.useStencil)
    {
        passDescriptor.stencilAttachment.clearStencil = begin->depthStencilAttachment.clearStencil;
        passDescriptor.stencilAttachment.loadAction = GetMTLLoadOp(begin->depthStencilAttachment.stencilLoadOp);
        passDescriptor.stencilAttachment.storeAction = GetMTLStoreOp(begin->depthStencilAttachment.stencilStoreOp);
        
        id<MTLTexture> stencil = AS_MTL(m_textures.GetItemR(begin->depthStencilAttachment.texture).ptr, id<MTLTexture>);
        passDescriptor.stencilAttachment.texture = stencil;
    }
    
    for(uint32 i = 0; i < begin->colorAttachmentCount; i++)
    {
        const auto& att = begin->colorAttachments[i];
        passDescriptor.colorAttachments[i].loadAction = GetMTLLoadOp(att.loadOp);
        passDescriptor.colorAttachments[i].storeAction = GetMTLStoreOp(att.storeOp);
        passDescriptor.colorAttachments[i].clearColor = MTLClearColorMake(att.clearColor.x, att.clearColor.y, att.clearColor.z, att.clearColor.w);
        
        if(att.isSwapchain)
        {
            const uint8 swpHandle = static_cast<uint8>(att.texture);
            const auto& swp = m_swapchains.GetItemR(swpHandle);
            id<CAMetalDrawable> drawable = AS_MTL(swp._currentDrawable, id<CAMetalDrawable>);
            passDescriptor.colorAttachments[i].texture = drawable.texture;
            stream.writtenSwapchains.push_back(swpHandle);
        }
        else
        {
            id<MTLTexture> txt = AS_MTL(m_textures.GetItemR(att.texture).ptr, id<MTLTexture>);
            passDescriptor.colorAttachments[i].texture = txt;
        }
    }
    
    id<MTLRenderCommandEncoder> encoder = [buffer renderCommandEncoderWithDescriptor:passDescriptor];
    [encoder retain];

  
    MTLViewport vp;
    vp.width = begin->viewport.width;
    vp.height = begin->viewport.height;
    vp.originX = begin->viewport.x;
    vp.originY = begin->viewport.y;
    vp.znear = begin->viewport.minDepth;
    vp.zfar = begin->viewport.maxDepth;
    
    MTLScissorRect sc;
    sc.width = begin->scissors.width;
    sc.height = begin->scissors.height;
    sc.x = begin->scissors.x;
    sc.y = begin->scissors.y;
    
    [encoder setScissorRect:sc];
    [encoder setViewport:vp];
    stream.currentEncoder = AS_VOID(encoder);
    stream.allRenderEncoders.push_back(stream.currentEncoder);
    
    if(stream.containsDelayedVtxBind)
    {
        stream.containsDelayedVtxBind = false;
        CMD_BindVertexBuffers((uint8*)(&stream.delayedVtxBind), stream);
    }
    
    BindDescriptorSets(stream);
}

void MTLBackend::CMD_EndRenderPass(uint8 *data, MTLCommandStream &stream) {
    CMDEndRenderPass* end  = reinterpret_cast<CMDEndRenderPass*>(data);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    [encoder endEncoding];
    [encoder release];
}

void MTLBackend::CMD_SetViewport(uint8 *data, MTLCommandStream &stream) {
    CMDSetViewport* cmd  = reinterpret_cast<CMDSetViewport*>(data);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    MTLViewport vp;
    vp.width = cmd->width;
    vp.height = cmd->height;
    vp.originX = cmd->x;
    vp.originY = cmd->y;
    vp.znear = cmd->minDepth;
    vp.zfar = cmd->maxDepth;
    [encoder setViewport:vp];
}

void MTLBackend::CMD_SetScissors(uint8 *data, MTLCommandStream &stream) {
    CMDSetScissors* cmd  = reinterpret_cast<CMDSetScissors*>(data);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    MTLScissorRect sc;
    sc.width = cmd->width;
    sc.height = cmd->height;
    sc.x = cmd->x;
    sc.y = cmd->y;
    [encoder setScissorRect:sc];
}

void MTLBackend::CMD_BindPipeline(uint8 *data, MTLCommandStream &stream) {
    CMDBindPipeline* cmd  = reinterpret_cast<CMDBindPipeline*>(data);
    const auto& shader = m_shaders.GetItemR(cmd->shader);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    id<MTLComputeCommandEncoder> computeEncoder = AS_MTL(stream.currentComputeEncoder, id<MTLComputeCommandEncoder>);
    
    if(shader.isCompute)
    {
        [computeEncoder setComputePipelineState:AS_MTL(shader.cso, id<MTLComputePipelineState>)];
    }
    else
    {
        [encoder setRenderPipelineState:AS_MTL(shader.pso, id<MTLRenderPipelineState>)];
        
       if(stream.currentRenderPassUseDepth && shader.dsso != nullptr && shader.dsso != stream.currentEncoderDepthStencil)
       {
           [encoder setDepthStencilState:AS_MTL(shader.dsso, id<MTLDepthStencilState>)];
           stream.currentEncoderDepthStencil = shader.dsso;
       }
        
        [encoder setCullMode:GetMTLCullMode(shader.cullMode)];
        [encoder setFrontFacingWinding:shader.frontFace == FrontFace::CW ? MTLWindingClockwise : MTLWindingCounterClockwise];
    }
    
    stream.currentShader = cmd->shader;
    stream.currentShaderIsCompute = shader.isCompute;
    stream.currentShaderExists = true;
    
    for (auto& set : stream.boundDescriptorSets)
        set.second.isDirty = true;
    
    BindDescriptorSets(stream);
}

void MTLBackend::CMD_DrawInstanced(uint8 *data, MTLCommandStream &stream) {
    CMDDrawInstanced* cmd  = reinterpret_cast<CMDDrawInstanced*>(data);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    const auto& shader = m_shaders.GetItemR(stream.currentShader);
    [encoder drawPrimitives:GetMTLPrimitive(shader.topology) vertexStart:cmd->startVertexLocation vertexCount:cmd->vertexCountPerInstance instanceCount:cmd->instanceCount];
}

void MTLBackend::CMD_DrawIndexedInstanced(uint8 *data, MTLCommandStream &stream) {
    CMDDrawIndexedInstanced* cmd  = reinterpret_cast<CMDDrawIndexedInstanced*>(data);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    const auto& resource = m_resources.GetItemR(stream.currentIndexBuffer);
    id<MTLBuffer> buffer = AS_MTL(resource.ptr, id<MTLBuffer>);
    const auto& shader = m_shaders.GetItemR(stream.currentShader);

    auto indexBufferType = stream.indexBufferType == 0 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
    auto iboffset = stream.indexBufferType == 0 ? (cmd->startIndexLocation * sizeof(uint16)) : (cmd->startIndexLocation * sizeof(uint32));
    [encoder drawIndexedPrimitives:GetMTLPrimitive(shader.topology) indexCount:cmd->indexCountPerInstance indexType:indexBufferType indexBuffer:buffer indexBufferOffset:iboffset  instanceCount:cmd->instanceCount baseVertex:cmd->baseVertexLocation baseInstance:cmd->startInstanceLocation];
}

void MTLBackend::CMD_DrawIndexedIndirect(uint8 *data, MTLCommandStream &stream) {
    CMDDrawIndexedIndirect* cmd    = reinterpret_cast<CMDDrawIndexedIndirect*>(data);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    const auto& shader = m_shaders.GetItemR(stream.currentShader);
    const auto& resource = m_resources.GetItemR(cmd->indirectBuffer);
    id<MTLBuffer> indirectBuffer= AS_MTL(resource.ptr, id<MTLBuffer>);
    
    if(stream.indirectCommandBuffer != nullptr)
    {
        if(stream.indirectCommandBufferMaxCommands < cmd->count * 2)
        {
            id<MTLIndirectCommandBuffer> buf = AS_MTL(stream.indirectCommandBuffer, id<MTLIndirectCommandBuffer>);
            [buf release];
            stream.indirectCommandBuffer = nullptr;
        }
    }
   
    if(stream.indirectCommandBuffer == nullptr)
    {
        auto device = AS_MTL(m_device, id<MTLDevice>);
        MTLIndirectCommandBufferDescriptor * descriptor = [[MTLIndirectCommandBufferDescriptor alloc] init];
        descriptor.commandTypes = MTLIndirectCommandTypeDrawIndexed;
        descriptor.inheritBuffers = YES;
        descriptor.inheritPipelineState = YES;
        
        id<MTLIndirectCommandBuffer> buf = [device newIndirectCommandBufferWithDescriptor:descriptor maxCommandCount:cmd->count * 2 options:0];
        [buf retain];
        stream.indirectCommandBuffer = AS_VOID(buf);
        stream.indirectCommandBufferMaxCommands = cmd->count*2;
    }
   
    id<MTLIndirectCommandBuffer> indCmd = AS_MTL(stream.indirectCommandBuffer, id<MTLIndirectCommandBuffer>);
    
    const auto& indexRes = m_resources.GetItemR(stream.currentIndexBuffer);
    id<MTLBuffer> indexBuf = AS_MTL(indexRes.ptr, id<MTLBuffer>);
    
    // LGXDrawID is fed into the Nth element so offset by it.
    for(int32 i = 0; i < cmd->count; i++)
    {
        if(shader.layout.hasGLDrawID)
        {
            const int32 drawId = i;
            [encoder setVertexBytes:&drawId length:sizeof(uint32) atIndex:shader.layout.drawIDBinding];
        }
        
        auto indexBufferType = stream.indexBufferType == 0 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
        [encoder drawIndexedPrimitives:GetMTLPrimitive(shader.topology) indexType:indexBufferType indexBuffer:indexBuf indexBufferOffset:0 indirectBuffer:indirectBuffer indirectBufferOffset:sizeof(IndexedIndirectCommand) * i + sizeof(uint32)];
    }
}

void MTLBackend::CMD_DrawIndirect(uint8 *data, MTLCommandStream &stream) {
    CMDDrawIndirect* cmd    = reinterpret_cast<CMDDrawIndirect*>(data);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    const auto& shader = m_shaders.GetItemR(stream.currentShader);
    const auto& resource = m_resources.GetItemR(cmd->indirectBuffer);
    id<MTLBuffer> indirectBuffer= AS_MTL(resource.ptr, id<MTLBuffer>);
    
    if(stream.indirectCommandBuffer != nullptr)
    {
        if(stream.indirectCommandBufferMaxCommands < cmd->count * 2)
        {
            id<MTLIndirectCommandBuffer> buf = AS_MTL(stream.indirectCommandBuffer, id<MTLIndirectCommandBuffer>);
            [buf release];
            stream.indirectCommandBuffer = nullptr;
        }
    }
   
    if(stream.indirectCommandBuffer == nullptr)
    {
        auto device = AS_MTL(m_device, id<MTLDevice>);
        MTLIndirectCommandBufferDescriptor * descriptor = [[MTLIndirectCommandBufferDescriptor alloc] init];
        descriptor.commandTypes = MTLIndirectCommandTypeDrawIndexed;
        descriptor.inheritBuffers = YES;
        descriptor.inheritPipelineState = YES;
        
        id<MTLIndirectCommandBuffer> buf = [device newIndirectCommandBufferWithDescriptor:descriptor maxCommandCount:cmd->count * 2 options:0];
        [buf retain];
        stream.indirectCommandBuffer = AS_VOID(buf);
        stream.indirectCommandBufferMaxCommands = cmd->count*2;
    }
   
    id<MTLIndirectCommandBuffer> indCmd = AS_MTL(stream.indirectCommandBuffer, id<MTLIndirectCommandBuffer>);
    
    const auto& indexRes = m_resources.GetItemR(stream.currentIndexBuffer);
    id<MTLBuffer> indexBuf = AS_MTL(indexRes.ptr, id<MTLBuffer>);
    
    // LGXDrawID is fed into the Nth element so offset by it.
    for(int32 i = 0; i < cmd->count; i++)
    {
        if(shader.layout.hasGLDrawID)
        {
            const int32 drawId = i;
            [encoder setVertexBytes:&drawId length:sizeof(uint32) atIndex:shader.layout.drawIDBinding];
        }
        
        auto indexBufferType = stream.indexBufferType == 0 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
        [encoder drawIndexedPrimitives:GetMTLPrimitive(shader.topology) indexType:indexBufferType indexBuffer:indexBuf indexBufferOffset:0 indirectBuffer:indirectBuffer indirectBufferOffset:sizeof(IndexedIndirectCommand) * i + sizeof(uint32)];
    }
}


void MTLBackend::CMD_BindVertexBuffers(uint8 *data, MTLCommandStream &stream) {
    CMDBindVertexBuffers* cmd      = reinterpret_cast<CMDBindVertexBuffers*>(data);
    
    if(stream.currentEncoder == nullptr)
    {
        stream.containsDelayedVtxBind = true;
        stream.delayedVtxBind = *cmd;
        return;
    }
    
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    const auto& resource = m_resources.GetItemR(cmd->resource);
    id<MTLBuffer> buffer = AS_MTL(resource.ptr, id<MTLBuffer>);
    [encoder setVertexBuffer:buffer offset:cmd->offset atIndex:cmd->slot];
}

void MTLBackend::CMD_BindIndexBuffers(uint8 *data, MTLCommandStream &stream) {
    CMDBindIndexBuffers*    cmd  = reinterpret_cast<CMDBindIndexBuffers*>(data);
    stream.currentIndexBuffer = cmd->resource;
    stream.indexBufferType = static_cast<uint8>(cmd->indexType);
}

void MTLBackend::CMD_CopyResource(uint8 *data, MTLCommandStream &stream) {
    CMDCopyResource* cmd     = reinterpret_cast<CMDCopyResource*>(data);
    id<MTLBlitCommandEncoder> encoder = AS_MTL(stream.currentBlitEncoder, id<MTLBlitCommandEncoder>);
    const auto& srcResource = m_resources.GetItemR(cmd->source);
    const auto& dstResource = m_resources.GetItemR(cmd->destination);
    id<MTLBuffer> srcBuffer = AS_MTL(srcResource.ptr, id<MTLBuffer>);
    id<MTLBuffer> dstBuffer = AS_MTL(dstResource.ptr, id<MTLBuffer>);
    [encoder copyFromBuffer:srcBuffer sourceOffset:0 toBuffer:dstBuffer destinationOffset:0 size:srcResource.size];
}

void MTLBackend::CMD_CopyBufferToTexture2D(uint8 *data, MTLCommandStream &stream) {
    CMDCopyBufferToTexture2D* cmd        = reinterpret_cast<CMDCopyBufferToTexture2D*>(data);
    id<MTLBlitCommandEncoder> encoder = AS_MTL(stream.currentBlitEncoder, id<MTLBlitCommandEncoder>);
    
    const auto& txtResource = m_textures.GetItemR(cmd->destTexture);
    id<MTLTexture> destTexture = AS_MTL(txtResource.ptr, id<MTLTexture>);
    
    uint32 totalSize = 0;
    
    for(uint32 i = 0; i < cmd->mipLevels; i++)
    {
        const auto& buffer = cmd->buffers[i];
        totalSize += buffer.width * buffer.height * buffer.bytesPerPixel;
    }
    
    ResourceDesc desc = {
        .size = totalSize,
        .typeHintFlags = ResourceTypeHint::TH_None,
        .heapType = ResourceHeap::StagingHeap,
    };
    
    uint32 intermediateResource = CreateResource(desc);
    
    uint8* mappedData = nullptr;
    MapResource(intermediateResource, mappedData);
    
    size_t offset = 0;
    for(uint32 i = 0; i < cmd->mipLevels; i++)
    {
        const auto& buffer = cmd->buffers[i];
        const size_t sz = static_cast<size_t>(buffer.width * buffer.height * buffer.bytesPerPixel);
        LINAGX_MEMCPY(mappedData + offset, buffer.pixels, sz);
        offset += sz;
    }

    UnmapResource(intermediateResource);
    
    id<MTLBuffer> intermediateBuffer = AS_MTL(m_resources.GetItemR(intermediateResource).ptr, id<MTLBuffer>);
    
    offset = 0;
    for(uint32 i = 0; i < cmd->mipLevels; i++)
    {
        const auto& buffer = cmd->buffers[i];
        uint32 mipSize = buffer.width * buffer.height * buffer.bytesPerPixel;

        auto srcSize = (MTLSize){buffer.width, buffer.height, 1};
        auto dstOrigin = (MTLOrigin){0,0,0};
        [encoder copyFromBuffer:intermediateBuffer sourceOffset:offset sourceBytesPerRow:buffer.width * buffer.bytesPerPixel sourceBytesPerImage:mipSize sourceSize:srcSize toTexture:destTexture destinationSlice:cmd->destinationSlice destinationLevel:i destinationOrigin:dstOrigin];
        offset += mipSize;
    }
    
    stream.intermediateResources[intermediateResource] = PerformanceStats.totalFrames;
}

void MTLBackend::CMD_BindDescriptorSets(uint8 *data, MTLCommandStream &stream) {
    CMDBindDescriptorSets* cmd    = reinterpret_cast<CMDBindDescriptorSets*>(data);
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    id<MTLComputeCommandEncoder> computeEncoder = AS_MTL(stream.currentComputeEncoder, id<MTLComputeCommandEncoder>);

    uint32 dynCtr = 0;
    for(uint32 i = 0; i < cmd->setCount; i++)
    {
        const uint16 setHandle = cmd->descriptorSetHandles[i];
        const uint32 setIndex = i + cmd->firstSet;
        const auto& set = m_descriptorSets.GetItemR(setHandle);
        MTLBoundDescriptorSet data = {setHandle, true};
        
        for(const auto& binding : set.bindings)
        {
            if(binding.lgxBinding.useDynamicOffset)
                data.dynamicOffsets.push_back(cmd->dynamicOffsets[dynCtr++]);
        }
        
        stream.boundSets[setIndex] = data;

    }
    
    BindDescriptorSets(stream);
}

void MTLBackend::CMD_BindConstants(uint8 *data, MTLCommandStream &stream) {
    CMDBindConstants* cmd    = reinterpret_cast<CMDBindConstants*>(data);
    
    id<MTLRenderCommandEncoder> encoder = AS_MTL(stream.currentEncoder, id<MTLRenderCommandEncoder>);
    id<MTLComputeCommandEncoder> computeEncoder = AS_MTL(stream.currentComputeEncoder, id<MTLComputeCommandEncoder>);
    auto& shader = m_shaders.GetItemR(stream.currentShader);
    
    uint8* finalData = nullptr;
    if(cmd->offset != 0)
    {
        uint8* bytes = (uint8*)malloc(static_cast<size_t>(cmd->size + cmd->offset));
        uint8* dummy = (uint8*)malloc(static_cast<size_t>(cmd->offset));
        std::memcpy(bytes, dummy, cmd->offset);
        std::memcpy(bytes + cmd->offset, cmd->data, static_cast<size_t>(cmd->size));
        finalData = bytes;
    }
    else
        finalData = (uint8*)cmd->data;
    
    if(shader.isCompute)
    {
        uint32 constantIndex = shader.layout.constantsMSLBuffers[ShaderStage::Compute];
        [computeEncoder setBytes:finalData length:cmd->size + cmd->offset atIndex:constantIndex];
    }
    else
    {
        
        for (uint32 i = 0; i < cmd->stagesSize; ++i)
        {
            ShaderStage stage = cmd->stages[i];
            uint32 constantIndex = shader.layout.constantsMSLBuffers[stage];
            if (stage == ShaderStage::Vertex) {
                [encoder setVertexBytes:finalData length:cmd->size + cmd->offset atIndex:constantIndex];
            } else if (stage == ShaderStage::Fragment) {
                [encoder setFragmentBytes:finalData length:cmd->size + cmd->offset atIndex:constantIndex];
            }
        }
    }

}

void MTLBackend::CMD_Dispatch(uint8 *data, MTLCommandStream &stream) {
    CMDDispatch* cmd  = reinterpret_cast<CMDDispatch*>(data);
    id<MTLComputeCommandEncoder> computeEncoder = AS_MTL(stream.currentComputeEncoder, id<MTLComputeCommandEncoder>);

    MTLSize threadgroups = MTLSizeMake(cmd->groupSizeX, cmd->groupSizeY, cmd->groupSizeZ);
    MTLSize threadsPerThreadgroup = MTLSizeMake(8,8,8);

    // Dispatching the compute command
    [computeEncoder dispatchThreadgroups:threadgroups threadsPerThreadgroup:threadsPerThreadgroup];
}

void MTLBackend::CMD_ComputeBarrier(uint8 *data, MTLCommandStream &stream) {
    CMDComputeBarrier* cmd  = reinterpret_cast<CMDComputeBarrier*>(data);
}

void MTLBackend::CMD_ExecuteSecondaryStream(uint8 *data, MTLCommandStream &stream) {
    CMDExecuteSecondaryStream* cmd  = reinterpret_cast<CMDExecuteSecondaryStream*>(data);
}

void MTLBackend::CMD_Barrier(LinaGX::uint8 *data, LinaGX::MTLCommandStream &stream) { 
    CMDBarrier* cmd = reinterpret_cast<CMDBarrier*>(data);
    
}




} // namespace LinaVG

