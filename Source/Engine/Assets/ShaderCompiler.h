// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/String.h"
#include "RHI/RHI.h"

namespace Alimer::ShaderCompiler
{
    struct ShaderCompileOptions {
        std::string source;
        std::string entryPoint = "main";
        std::string fileName = "";
        std::vector<std::string> defines;
        RHI::ShaderStage stage;
        RHI::ShaderModel shaderModel = RHI::ShaderModel::Model6_0;
    };

    //ALIMER_API ShaderRef Compile(const std::string& fileName, ShaderBlobType blobType);
    //ALIMER_API ShaderRef Compile(ShaderStage stage, const std::string& fileName, ShaderBlobType blobType);
    ALIMER_API bool Compile(const ShaderCompileOptions& options, RHI::Shader* shader);
}
