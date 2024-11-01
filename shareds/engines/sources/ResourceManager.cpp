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

std::shared_ptr<AssimpPack> AssimpPack::LoadAsync(std::wstring path, std::wstring name)
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
                aiProcess_FlipWindingOrder | //CW, CCW �ٲٴ°���.
                aiProcess_FlipUVs | // ���״�� uv�� y���� ������. �׸��� bitangent�� ������.

                aiProcess_Triangulate | // 4���� 5������ 3��������
                //aiProcess_GenSmoothNormals | // Normal�� ������ Smmoth Normal ����
                aiProcess_GenNormals | // Normal�� ������ Normal ����
                aiProcess_ImproveCacheLocality |// �ﰢ�� ����. �� �Ǹ� �Ѻ��� ĳ����Ʈ���� ���� �ﰢ�� ��������.
                aiProcess_GenUVCoords | // UV������ UV ����ϰ�[ ��Ű��
                aiProcess_CalcTangentSpace | // ź��Ʈ ���
                //aiProcess_SplitLargeMeshes |// �Ž��� �ʹ� Ŭ�� �ɰ��°� �Ž� Ŭ�� ������ ����.
                //aiProcess_Debone | �սǾ��� �� ����. �� ��������.
                //aiProcess_RemoveComponent | // (animations, materials, light sources, cameras, textures, vertex components ����
                //aiProcess_PreTransformVertices | // root Node�� ������ ��� ���� ���� ���� ��źȭ. ���� ����.
                //aiProcess_ValidateDataStructure | // ���� ��ȿ�� �˻�
                //aiProcess_RemoveRedundantMaterials | // �ߺ��̳� �Ⱦ��°� ����
                aiProcess_FixInfacingNormals | //�߸� ����Ǽ� ���峭 ��� ���� ����
                //aiProcess_FindDegenerates | //�ﰢ������ ���� ���Ĺ����� �����̳� ���� �ǹ����µ�, �̰� Line�̳� Point�� ��ȯ�ϴ°���. �Ⱦ��°� ����.
                aiProcess_FindInvalidData | //��ȿ���� �ʴ� ������ ������ ����, ���� UV�� ������. �̷��� �����ϰ� ���� aiProcess_GenNormals������ ���Ӱ� �������ٰ���. �ִϸ��̼ǿ����� ������ �ִٰ���.
                //aiProcess_GenUVCoords  | //UV�� ��ü������ �����. �𵨸������� �����ϴ°� ��õ�ϰ�, UV�� ������ ���Ӱ� �����ϴ°���.
                //aiProcess_FindInstances | //�ʹ� �Ž��� ������ Ű����. �����ٴ°Ű���. ���� ������ �Ž����� �ϳ��� ���Ĺ����� ����ε�.
                //aiProcess_OptimizeMeshes |// �Ž� �� �ٿ��ִ� ����ȭ �ɼ��ε�. aiProcess_OptimizeGraph�� ���� ���°� ����, #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.�� ȣȯ��.
                //�� ��Ű�°� ������. ���� �ؿ� �ɼ��̶� ȣȯ�Ǵ� ����ε�, �ؿ� �ɼ��� ����.
                //aiProcess_OptimizeGraph |//�ʿ���� ��带 ������. ��尡 �±׷� ���϶� �����Ǵ� ������ �ճ���, ��Ű�°� ������. ���������� �սǵȴٰ� ��.
                //aiProcess_TransformUVCoords | //UV�� ���ؼ� ��ȯó�� �Ѵٰ� �ϴ°Ű���. �ؽ��� �̻������� ����������
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
    return shared_from_this();
}

void ResourceManager::Init()
{
    
    
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
    return true;
}

std::shared_ptr<AssimpPack> ResourceManager::LoadAssimpPack(const std::wstring& path, const std::wstring& name)
{
    auto pack = std::make_shared<AssimpPack>()->Init()->LoadAsync(path, name);
    assimpPackList.push_back(pack);
    assimpPackTable[name] = pack;
    return pack;
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

std::shared_ptr<Texture> ResourceManager::LoadTexture(std::wstring path, std::wstring name)
{
    return nullptr;
}


std::shared_ptr<Shader> ResourceManager::LoadShader(std::wstring path, std::wstring name)
{
    return nullptr;
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

std::shared_ptr<Shader> ResourceManager::GetTexture(std::wstring name)
{
    return nullptr;
}

std::shared_ptr<Texture> ResourceManager::GetShader(std::wstring name)
{
    return nullptr;
}
