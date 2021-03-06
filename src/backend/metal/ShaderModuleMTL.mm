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

#include "backend/metal/ShaderModuleMTL.h"

#include "backend/metal/MetalBackend.h"
#include "backend/metal/PipelineLayoutMTL.h"

#include <spirv-cross/spirv_msl.hpp>

#include <sstream>

namespace backend { namespace metal {

    namespace {

        spv::ExecutionModel SpirvExecutionModelForStage(nxt::ShaderStage stage) {
            switch (stage) {
                case nxt::ShaderStage::Vertex:
                    return spv::ExecutionModelVertex;
                case nxt::ShaderStage::Fragment:
                    return spv::ExecutionModelFragment;
                case nxt::ShaderStage::Compute:
                    return spv::ExecutionModelGLCompute;
                default:
                    UNREACHABLE();
            }
        }
    }

    ShaderModule::ShaderModule(ShaderModuleBuilder* builder)
        : ShaderModuleBase(builder), mSpirv(builder->AcquireSpirv()) {
        spirv_cross::CompilerMSL compiler(mSpirv);
        ExtractSpirvInfo(compiler);
    }

    ShaderModule::MetalFunctionData ShaderModule::GetFunction(const char* functionName,
                                                              const PipelineLayout* layout) const {
        spirv_cross::CompilerMSL compiler(mSpirv);

        spirv_cross::CompilerGLSL::Options options_glsl;
        options_glsl.vertex.flip_vert_y = true;
        compiler.spirv_cross::CompilerGLSL::set_options(options_glsl);

        // By default SPIRV-Cross will give MSL resources indices in increasing order.
        // To make the MSL indices match the indices chosen in the PipelineLayout, we build
        // a table of MSLResourceBinding to give to SPIRV-Cross
        std::vector<spirv_cross::MSLResourceBinding> mslBindings;

        // Reserve index 0 for buffers for the push constants buffer.
        for (auto stage : IterateStages(kAllStages)) {
            spirv_cross::MSLResourceBinding binding;
            binding.stage = SpirvExecutionModelForStage(stage);
            binding.desc_set = spirv_cross::kPushConstDescSet;
            binding.binding = spirv_cross::kPushConstBinding;
            binding.msl_buffer = 0;

            mslBindings.push_back(binding);
        }

        // Create one resource binding entry per stage per binding.
        for (uint32_t group : IterateBitSet(layout->GetBindGroupsLayoutMask())) {
            const auto& bgInfo = layout->GetBindGroupLayout(group)->GetBindingInfo();
            for (uint32_t binding : IterateBitSet(bgInfo.mask)) {
                for (auto stage : IterateStages(bgInfo.visibilities[binding])) {
                    uint32_t index = layout->GetBindingIndexInfo(stage)[group][binding];

                    spirv_cross::MSLResourceBinding mslBinding;
                    mslBinding.stage = SpirvExecutionModelForStage(stage);
                    mslBinding.desc_set = group;
                    mslBinding.binding = binding;
                    mslBinding.msl_buffer = mslBinding.msl_texture = mslBinding.msl_sampler = index;

                    mslBindings.push_back(mslBinding);
                }
            }
        }

        MetalFunctionData result;

        {
            auto size = compiler.get_entry_point(functionName).workgroup_size;
            result.localWorkgroupSize = MTLSizeMake(size.x, size.y, size.z);
        }

        {
            // SPIRV-Cross also supports re-ordering attributes but it seems to do the correct thing
            // by default.
            std::string msl = compiler.compile(nullptr, &mslBindings);
            NSString* mslSource = [NSString stringWithFormat:@"%s", msl.c_str()];

            auto mtlDevice = ToBackend(GetDevice())->GetMTLDevice();
            NSError* error = nil;
            id<MTLLibrary> library =
                [mtlDevice newLibraryWithSource:mslSource options:nil error:&error];
            if (error != nil) {
                // TODO(cwallez@chromium.org): forward errors to caller
                NSLog(@"MTLDevice newLibraryWithSource => %@", error);
            }
            // TODO(kainino@chromium.org): make this somehow more robust; it needs to behave like
            // clean_func_name:
            // https://github.com/KhronosGroup/SPIRV-Cross/blob/4e915e8c483e319d0dd7a1fa22318bef28f8cca3/spirv_msl.cpp#L1213
            if (strcmp(functionName, "main") == 0) {
                functionName = "main0";
            }

            NSString* name = [NSString stringWithFormat:@"%s", functionName];
            result.function = [library newFunctionWithName:name];
            [library release];
        }

        return result;
    }

}}  // namespace backend::metal
