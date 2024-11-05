#include "stdafx.h"
#include "ResourceManager.h"

#include "Model.h"


ResourceManager* ResourceManager::main = nullptr;

ResourceManager::ResourceManager()
{
    if (main == nullptr)
        main = this;
}

ResourceManager::~ResourceManager()
{
    Release();
}

AssimpPack::AssimpPack()
{
}

AssimpPack::~AssimpPack()
{
}

std::shared_ptr<AssimpPack> AssimpPack::Init()
{
    importer = std::make_shared<Assimp::Importer>();

    return shared_from_this();
}

std::shared_ptr<AssimpPack> AssimpPack::Load(std::wstring path, std::wstring name, bool async)
{
    auto pathString = std::to_string(path);
    this->name = name;
    this->path = path;
    this->asyncLoadThread = std::make_shared<std::jthread>(std::bind([=](AssimpPack* pack, std::stop_token token, std::string path)
    {
        pack->loadComplate.store(false);
        try
        {
            Debug::log << "�� �ε� �� : " << path << "\n";
            pack->scene = pack->importer->ReadFile(path + "\0",
                
                aiProcess_MakeLeftHanded | // �޼� ��ǥ��� ����
                //aiProcess_ConvertToLeftHanded |
                aiProcess_FlipWindingOrder | //CW, CCW �ٲٴ°���.
                aiProcess_FlipUVs | // ���״�� uv�� y���� ������. �׸��� bitangent�� ������.

                aiProcess_Triangulate | // 4���� 5������ 3��������
                //aiProcess_GenSmoothNormals | // Normal�� ������ Smmoth Normal ����
                aiProcess_GenNormals | // Normal�� ������ Normal ����
                 aiProcess_ImproveCacheLocality |// �ﰢ�� ����. �� �Ǹ� �Ѻ��� ĳ����Ʈ���� ���� �ﰢ�� ��������.
                //aiProcess_GenUVCoords | // UV������ UV ����ϰ�[ ��Ű��
                aiProcess_CalcTangentSpace | // ź��Ʈ ���
                //aiProcess_SplitLargeMeshes |// �Ž��� �ʹ� Ŭ�� �ɰ��°� �Ž� Ŭ�� ������ ����.
                //aiProcess_Debone | �սǾ��� �� ����. �� ��������.
                //aiProcess_RemoveComponent | // (animations, materials, light sources, cameras, textures, vertex components ����
                //aiProcess_PreTransformVertices | // root Node�� ������ ��� ���� ���� ���� ��źȭ. ���� ����.
                //aiProcess_ValidateDataStructure | // ���� ��ȿ�� �˻�
                //aiProcess_RemoveRedundantMaterials | // �ߺ��̳� �Ⱦ��°� ����
                 //aiProcess_FixInfacingNormals | //�߸� ����Ǽ� ���峭 ��� ���� ����
                //aiProcess_FindDegenerates | //�ﰢ������ ���� ���Ĺ����� �����̳� ���� �ǹ����µ�, �̰� Line�̳� Point�� ��ȯ�ϴ°���. �Ⱦ��°� ����.
                 //aiProcess_FindInvalidData | //��ȿ���� �ʴ� ������ ������ ����, ���� UV�� ������. �̷��� �����ϰ� ���� aiProcess_GenNormals������ ���Ӱ� �������ٰ���. �ִϸ��̼ǿ����� ������ �ִٰ���.
                //aiProcess_GenUVCoords  | //UV�� ��ü������ �����. �𵨸������� �����ϴ°� ��õ�ϰ�, UV�� ������ ���Ӱ� �����ϴ°���.
                //aiProcess_FindInstances | //�ʹ� �Ž��� ������ Ű����. �����ٴ°Ű���. ���� ������ �Ž����� �ϳ��� ���Ĺ����� ����ε�.
                //aiProcess_OptimizeMeshes |// �Ž� �� �ٿ��ִ� ����ȭ �ɼ��ε�. aiProcess_OptimizeGraph�� ���� ���°� ����, #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.�� ȣȯ��.
                //�� ��Ű�°� ������. ���� �ؿ� �ɼ��̶� ȣȯ�Ǵ� ����ε�, �ؿ� �ɼ��� ����.
                //aiProcess_OptimizeGraph |//�ʿ���� ��带 ������. ��尡 �±׷� ���϶� �����Ǵ� ������ �ճ���, ��Ű�°� ������. ���������� �սǵȴٰ� ��.
                aiProcess_TransformUVCoords | //UV�� ���ؼ� ��ȯó�� �Ѵٰ� �ϴ°Ű���. �ؽ��� �̻������� ����������
                aiProcess_JoinIdenticalVertices | // �ߺ����� �� �ε��� ���� ������� ��ȯ
                aiProcess_SortByPType // �������� Ÿ�Ժ��� ��������. aiProcess_Triangulate ���� ������ �ﰢ���� ���Ƽ� �ʿ� ����. �ϴ� �־�~ 
            );
            if ((pack->scene == nullptr) || pack->scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || (pack->scene->mRootNode == nullptr))
            {
                Debug::log << "�� �ε� ���� : " << path << "\n" << pack->importer->GetErrorString() << "\n";
            }
            else
            {
                Debug::log << "�� �ε� ���� : "<< path <<"\n";
                ResourceManager::main->LoadModel(pack->shared_from_this());
            }
        }
        catch (...)
        {
            Debug::log << "�� �ε� �� ���� : " << path <<"\n";
        }
        pack->loadComplate.store(true);
    }, this, std::placeholders::_1, pathString));
    if (!async)
        this->asyncLoadThread->join();
    return shared_from_this();
}

void ResourceManager::Init()
{
    
    
}

void ResourceManager::Release()
{
    modelList.clear();
    textureList.clear();
    shaderList.clear();
    assimpPackList.clear();

    modelTable.clear();
    textureTable.clear();
    shaderTable.clear();
    assimpPackTable.clear();
}

bool ResourceManager::IsLoading()
{
    bool loading = false;
    for (int i=0;i< assimpPackList.size();i++)
    {
        bool currentLoad = !assimpPackList[i]->loadComplate.load();
        loading |= currentLoad;
    }
    return loading;
}

bool ResourceManager::WaitAll()
{
    for (int i = 0; i < assimpPackList.size(); i++)
    {
        bool currentLoad = !assimpPackList[i]->loadComplate.load();
        if (currentLoad)
            assimpPackList[i]->asyncLoadThread->join();
    }
    for (int i = 0; i < texturePackList.size(); i++)
    {
        bool currentLoad = !texturePackList[i]->loadComplate.load();
        if (currentLoad)
            texturePackList[i]->asyncLoadThread->join();
    }
    return true;
}

std::shared_ptr<AssimpPack> ResourceManager::LoadAssimpPack(const std::wstring& path, const std::wstring& name, bool async)
{
    auto pack = std::make_shared<AssimpPack>()->Init()->Load(path, name, async);
    assimpPackList.push_back(pack);
    assimpPackTable[name] = pack;
    return pack;
}

std::shared_ptr<AssimpPack> ResourceManager::LoadAssimpPacks(std::vector<std::pair<std::wstring, std::wstring>> packs)
{
    return nullptr;
}

std::shared_ptr<AssimpPack> ResourceManager::LoadTextures(std::vector<std::pair<std::wstring, std::wstring>> packs, bool mipmap)
{
    return nullptr;
}

std::shared_ptr<Model> ResourceManager::LoadModel(std::shared_ptr<AssimpPack> pack)
{
    auto model = std::make_shared<Model>();
    model->SetName(pack->name);
    model->Init(pack->shared_from_this());
    modelList.push_back(model);
    modelTable[model->name] = model;
    return model;
}

std::shared_ptr<TexturePack> ResourceManager::LoadTexture(std::wstring path, std::wstring name, bool mipmap, bool async)
{
    auto texturePack = std::make_shared<TexturePack>();
    texturePackList.push_back(texturePack);
    texturePack->asyncLoadThread = std::make_shared<std::jthread>([=](std::shared_ptr<TexturePack> pack)
        {
            pack->loadComplate.store(false);
            auto texture = Texture::Load(path, true);
            ResourceManager::main->textureList.push_back(texture);
            ResourceManager::main->textureTable[name] = texture;
            pack->texture = texture.get();
            pack->loadComplate.store(true);
        }, texturePack);
    if (!async)
        texturePack->asyncLoadThread->join();
    return texturePack;
}


std::shared_ptr<Shader> ResourceManager::LoadShader(std::wstring path, std::wstring name, std::vector<std::shared_ptr<RenderTexture>> rtgs)
{
    auto shader = Shader::Load(path);
    shader->SetRenderTargets(rtgs);
    shaderTable[name] = shader;
    shaderList.push_back(shader);
    return shader;
}

std::shared_ptr<Model> ResourceManager::GetModel(std::wstring name)
{
    if (modelTable.contains(name))
        return modelTable[name];
    return nullptr;
}

std::shared_ptr<AssimpPack> ResourceManager::GetAssimpPack(std::wstring name)
{
    if (assimpPackTable.contains(name))
        return assimpPackTable[name];
    return nullptr;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(std::wstring name)
{
    if (textureTable.contains(name))
        return textureTable[name];
    return nullptr;
}

std::shared_ptr<Shader> ResourceManager::GetShader(std::wstring name)
{
    if (shaderTable.contains(name))
        return shaderTable[name];
    return nullptr;
}
