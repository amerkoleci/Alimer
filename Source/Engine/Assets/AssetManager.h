// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Types.h"
#include "Core/Module.h"
#include <memory>
#include <string>
#include <vector>

namespace Alimer
{
	class Asset;

	class ALIMER_API AssetManager final : public Module<AssetManager>
	{
	public:
		/// Constructor.
		AssetManager(const String& rootDirectory);

		/// Return a resource by type and name. Load if not loaded yet. Return null if not found or if fails, unless SetReturnFailedResources(true) has been called. Can be called only from the main thread.
		//RefPtr<Object> Load(const TypeInfo* type, const std::string& name);

		/// Template version of returning a resource by name.
		//template <class T> RefPtr<T> Load(const std::string& name)
		//{
		//	return StaticCast<T>(Load(T::GetTypeInfoStatic(), name));
		//}

	private:
		String rootDirectory;
		//std::vector<std::unique_ptr<AssetLoader>> loaders;
		//std::unordered_map<StringId32, RefPtr<Object>> assets;
	};

	/** Provides easier access to Assets module. */
	ALIMER_API AssetManager& gAssets();
}
