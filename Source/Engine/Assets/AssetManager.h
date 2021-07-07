// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Object.h"
#include "Core/Module.h"

namespace Alimer
{
	class Asset;

	class ALIMER_API AssetManager final : public Module<AssetManager>
	{
	public:
		/// Constructor.
		AssetManager(const String& rootDirectory);

		/// Return a asset by type and name. Load if not loaded yet. Return null if not found or if fails.
		SharedPtr<Asset> Load(const TypeInfo* type, const StringView& name);

		/// Template version of returning a asset by name.
		template <class T> SharedPtr<T> Load(const std::string& name)
		{
            static_assert((std::is_base_of<Asset, T>::value), "Specified type is not a valid Asset.");

			return std::static_pointer_cast<T>(Load(T::GetTypeInfoStatic(), name));
		}

	private:
		String rootDirectory;
		std::unordered_map<StringId32, SharedPtr<Asset>> assets;
	};

	/** Provides easier access to Assets module. */
	ALIMER_API AssetManager& gAssets();
}
