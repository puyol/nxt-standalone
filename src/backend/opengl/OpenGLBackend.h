// Copyright 2017 The NXT Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BACKEND_OPENGL_OPENGLBACKEND_H_
#define BACKEND_OPENGL_OPENGLBACKEND_H_

#include "nxt/nxtcpp.h"

#include "backend/BindGroup.h"
#include "backend/BindGroupLayout.h"
#include "backend/BlendState.h"
#include "backend/Buffer.h"
#include "backend/DepthStencilState.h"
#include "backend/Device.h"
#include "backend/InputState.h"
#include "backend/Queue.h"
#include "backend/RenderPassDescriptor.h"
#include "backend/ToBackend.h"

#include "glad/glad.h"

namespace backend { namespace opengl {

    class BindGroup;
    class BindGroupLayout;
    class BlendState;
    class Buffer;
    class BufferView;
    class CommandBuffer;
    class ComputePipeline;
    class DepthStencilState;
    class Device;
    class InputState;
    class PersistentPipelineState;
    class PipelineLayout;
    class Queue;
    class RenderPassDescriptor;
    class RenderPipeline;
    class Sampler;
    class ShaderModule;
    class SwapChain;
    class Texture;
    class TextureView;

    struct OpenGLBackendTraits {
        using BindGroupType = BindGroup;
        using BindGroupLayoutType = BindGroupLayout;
        using BlendStateType = BlendState;
        using BufferType = Buffer;
        using BufferViewType = BufferView;
        using CommandBufferType = CommandBuffer;
        using ComputePipelineType = ComputePipeline;
        using DepthStencilStateType = DepthStencilState;
        using DeviceType = Device;
        using InputStateType = InputState;
        using PipelineLayoutType = PipelineLayout;
        using QueueType = Queue;
        using RenderPassDescriptorType = RenderPassDescriptor;
        using RenderPipelineType = RenderPipeline;
        using SamplerType = Sampler;
        using ShaderModuleType = ShaderModule;
        using SwapChainType = SwapChain;
        using TextureType = Texture;
        using TextureViewType = TextureView;
    };

    template <typename T>
    auto ToBackend(T&& common) -> decltype(ToBackendBase<OpenGLBackendTraits>(common)) {
        return ToBackendBase<OpenGLBackendTraits>(common);
    }

    // Definition of backend types
    class Device : public DeviceBase {
      public:
        BindGroupBase* CreateBindGroup(BindGroupBuilder* builder) override;
        BindGroupLayoutBase* CreateBindGroupLayout(BindGroupLayoutBuilder* builder) override;
        BlendStateBase* CreateBlendState(BlendStateBuilder* builder) override;
        BufferBase* CreateBuffer(BufferBuilder* builder) override;
        BufferViewBase* CreateBufferView(BufferViewBuilder* builder) override;
        CommandBufferBase* CreateCommandBuffer(CommandBufferBuilder* builder) override;
        ComputePipelineBase* CreateComputePipeline(ComputePipelineBuilder* builder) override;
        DepthStencilStateBase* CreateDepthStencilState(DepthStencilStateBuilder* builder) override;
        InputStateBase* CreateInputState(InputStateBuilder* builder) override;
        PipelineLayoutBase* CreatePipelineLayout(PipelineLayoutBuilder* builder) override;
        QueueBase* CreateQueue(QueueBuilder* builder) override;
        RenderPassDescriptorBase* CreateRenderPassDescriptor(
            RenderPassDescriptorBuilder* builder) override;
        RenderPipelineBase* CreateRenderPipeline(RenderPipelineBuilder* builder) override;
        ShaderModuleBase* CreateShaderModule(ShaderModuleBuilder* builder) override;
        SwapChainBase* CreateSwapChain(SwapChainBuilder* builder) override;
        TextureBase* CreateTexture(TextureBuilder* builder) override;
        TextureViewBase* CreateTextureView(TextureViewBuilder* builder) override;

        void TickImpl() override;

      private:
        ResultOrError<SamplerBase*> CreateSamplerImpl(
            const nxt::SamplerDescriptor* descriptor) override;
    };

    class BindGroup : public BindGroupBase {
      public:
        BindGroup(BindGroupBuilder* builder);
    };

    class BindGroupLayout : public BindGroupLayoutBase {
      public:
        BindGroupLayout(BindGroupLayoutBuilder* builder);
    };

    class Queue : public QueueBase {
      public:
        Queue(QueueBuilder* builder);

        // NXT API
        void Submit(uint32_t numCommands, CommandBuffer* const* commands);
    };

    class RenderPassDescriptor : public RenderPassDescriptorBase {
      public:
        RenderPassDescriptor(RenderPassDescriptorBuilder* builder);
    };

}}  // namespace backend::opengl

#endif  // BACKEND_OPENGL_OPENGLBACKEND_H_
