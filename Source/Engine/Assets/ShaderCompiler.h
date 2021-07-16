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
        ShaderStage stage;
        ShaderModel shaderModel = ShaderModel::Model6_0;
    };

    ALIMER_API bool Compile(const std::string& fileName, RHIShader* shader);
    ALIMER_API bool Compile(ShaderStage stage, const std::string& fileName, RHIShader* shader);
    ALIMER_API bool Compile(const ShaderCompileOptions& options, RHIShader* shader);
}
