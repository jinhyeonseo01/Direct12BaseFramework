#pragma once

#include <stdafx.h>

#include "Shader.h"
#include "Texture.h"

namespace dxe
{
    class AssimpLoadPack : public std::enable_shared_from_this<AssimpLoadPack>
    {
    public:
        std::wstring name;
        const aiScene* scene = nullptr;
        std::shared_ptr<Assimp::Importer> importer;

        AssimpLoadPack();
        virtual ~AssimpLoadPack();

        std::shared_ptr<AssimpLoadPack> Init();
        std::shared_ptr<AssimpLoadPack> LoadAsync(std::wstring path, std::wstring name);

    public:
        std::shared_ptr<std::jthread> asyncLoadThread;
        std::atomic<bool> loadComplate{ false };
    };

	class ResourceManager
	{
	public:
        static ResourceManager* main;

        std::shared_ptr<Model> modelList;
        std::shared_ptr<Texture> textureList;
        std::shared_ptr<Shader> shaderList;

	    std::vector<std::shared_ptr<AssimpLoadPack>> _aiPackList;

	public:
        ResourceManager();
        virtual ~ResourceManager();
        void Init();
        bool IsLoading();
        bool WaitAll();
        std::shared_ptr<AssimpLoadPack> LoadModel(std::wstring path, std::wstring name);
	    void LoadTexture(std::wstring path, std::wstring name);
	};
}
