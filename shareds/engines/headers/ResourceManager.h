#pragma once

#include <stdafx.h>

#include "Shader.h"
#include "Texture.h"

namespace dxe
{
    class AssimpPack : public std::enable_shared_from_this<AssimpPack>
    {
    public:
        std::wstring name;
        std::wstring path;
        const aiScene* scene = nullptr;
        std::shared_ptr<Assimp::Importer> importer;

        AssimpPack();
        virtual ~AssimpPack();

        std::shared_ptr<AssimpPack> Init();
        std::shared_ptr<AssimpPack> Load(std::wstring path, std::wstring name, bool async = false);

    public:
        std::shared_ptr<std::jthread> asyncLoadThread;
        std::atomic<bool> loadComplate{false};
    };

    class ResourceManager
    {
    public:
        static ResourceManager* main;

        std::vector<std::shared_ptr<Model>> modelList;
        std::vector<std::shared_ptr<Texture>> textureList;
        std::vector<std::shared_ptr<Shader>> shaderList;
        std::vector<std::shared_ptr<AssimpPack>> assimpPackList;

        std::unordered_map<std::wstring, std::shared_ptr<Model>> modelTable;
        std::unordered_map<std::wstring, std::shared_ptr<Texture>> textureTable;
        std::unordered_map<std::wstring, std::shared_ptr<Shader>> shaderTable;
        std::unordered_map<std::wstring, std::shared_ptr<AssimpPack>> assimpPackTable;

    public:
        ResourceManager();
        virtual ~ResourceManager();
        void Init();
        void Release();
        bool IsLoading();
        bool WaitAll();
        void Debug();


        std::shared_ptr<AssimpPack> LoadAssimpPack(const std::wstring& path, const std::wstring& name,
                                                   bool async = false);
        std::vector<std::shared_ptr<AssimpPack>> LoadAssimpPacks(
            std::vector<std::pair<std::wstring, std::wstring>> packPairs, bool async = false);
        std::shared_ptr<Model> LoadModel(std::shared_ptr<AssimpPack> pack);
        std::shared_ptr<Texture> LoadTexture(std::wstring path, std::wstring name, bool mipmap = false);
        std::vector<std::shared_ptr<Texture>> LoadTextures(
            std::vector<std::pair<std::wstring, std::wstring>> texturePairs, bool mipmap = false);
        std::shared_ptr<Shader> LoadShader(std::wstring path, std::wstring name,
                                           std::vector<std::shared_ptr<RenderTexture>> rtgs);

        std::shared_ptr<Model> GetModel(std::wstring name);
        std::shared_ptr<AssimpPack> GetAssimpPack(std::wstring name);
        std::shared_ptr<Texture> GetTexture(std::wstring name);
        std::shared_ptr<Shader> GetShader(std::wstring name);
    };
}
