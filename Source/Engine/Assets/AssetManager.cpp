// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Assets/AssetManager.h"
#include "Assets/Asset.h"
#include <filesystem>
#include <fstream>
#include "Core/Log.h"

namespace fs = std::filesystem;

namespace Alimer
{
	AssetManager::AssetManager(const String& rootDirectory)
		: rootDirectory{ rootDirectory }
	{
	}

    SharedPtr<Asset> AssetManager::Load(const TypeInfo* type, const StringView& name)
    {
        // Open stream first.
        fs::path path = fs::path(rootDirectory) / name;
        if (!fs::exists(path))
        {
            LOGW("Asset file '{}' doesn't exists", name);
            return {};
        }

        StringId32 nameHash(name);
        auto it = assets.find(name);
        if (it != assets.end())
        {
            return it->second;
        }

        // TODO: VirtualStream, CompressStream, Asset Package loading

        // Open stream
        //std::fstream stream;
        //stream.open(path, std::fstream::in | std::fstream::app);
        //FileStream stream(path, FileMode::OpenRead);

        SharedPtr<Asset> asset = Object::CreateObject<Asset>(type->GetType());
        if (asset == nullptr)
        {
            LOGE("Failed to create asset '{}', no factory registered", name);
            return {};
        }

        // TODO: Load

        if (asset == nullptr)
        {
            LOGE("Failed to load asset '{}'", name);
            return {};
        }

        assets[nameHash] = asset;
        return asset;
    }

	AssetManager& gAssets()
	{
		return AssetManager::Instance();
	}
}
