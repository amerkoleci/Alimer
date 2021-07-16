// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/RefPtr.h"
#include "Graphics/GPUResource.h"

namespace Alimer
{
	class ALIMER_API Shader : public GPUObject, public RefCounted
	{
	public:
		static ShaderRef Create(ShaderStage stage, const std::vector<uint8_t>& byteCode, const std::string& entryPoint = "main");

		ShaderStage GetStage() const noexcept { return stage; }
		const std::string& GetEntryPoint() const { return entryPoint; }
		const std::vector<uint8_t>& GetByteCode() const { return byteCode; }
		const std::vector<ShaderResource>& GetResources() const { return resources; }
		size_t GetHash() const { return hash; }

	protected:
		/// Constructor.
		Shader(ShaderStage stage, const std::vector<uint8_t>& byteCode, const std::string& entryPoint);

		ShaderStage stage;
		std::vector<uint8_t> byteCode;
		std::string entryPoint;
		std::vector<ShaderResource> resources;
		size_t hash = 0;
	};
}
