// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Assets/AssetManager.h"
//#include "IO/FileSystem.h"
//#include "IO/FileStream.h"
//#include "Core/Log.h"

namespace Alimer
{
	AssetManager::AssetManager(const String& rootDirectory)
		: rootDirectory{ rootDirectory }
	{
	}

#if TODO
    RefPtr<Object> AssetManager::Load(const TypeInfo* type, const std::string& name)
    {
        // Open stream first.
        FilePath path = Path::Join(rootDirectory, name);
        if (!File::Exists(path))
        {
            LOGW("Asset file '{}' doesn't exists", path);
            return {};
        }

        StringId32 nameHash(path);
        auto it = assets.find(name);
        if (it != assets.end())
        {
            return it->second;
        }

        // TODO: VirtualStream, CompressStream, Asset Package loading

        // Open stream
        FileStream stream(path, FileMode::OpenRead);

        RefPtr<Object> asset;
        for (auto i = loaders.rbegin(); i != loaders.rend(); ++i)
        {
            const auto loader = i->get();
            if (loader->GetTypeInfo() == type)
            {
                asset = loader->LoadAsset(stream);
                break;
            }
        }

        if (asset.IsNull())
        {
            LOGE("Failed to load asset '{}'", name);
            return {};
        }

        assets[nameHash] = asset;
        return asset;
    }

    void AssetManager::RegisterLoader(std::unique_ptr<AssetLoader> loader)
    {
        if (const auto i = std::find(loaders.begin(), loaders.end(), loader); i == loaders.end())
        {
            loaders.push_back(std::move(loader));
        }
    }
#endif // TODO


	AssetManager& gAssets()
	{
		return AssetManager::Instance();
	}
}
