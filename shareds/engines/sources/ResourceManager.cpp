#include "stdafx.h"
#include "ResourceManager.h"


ResourceManager* ResourceManager::main = nullptr;

ResourceManager::ResourceManager()
{
    if (main == nullptr)
        main = this;
}

ResourceManager::~ResourceManager()
{

}

AssimpLoadPack::AssimpLoadPack()
{
}

AssimpLoadPack::~AssimpLoadPack()
{
}

std::shared_ptr<AssimpLoadPack> AssimpLoadPack::Init()
{
    importer = std::make_shared<Assimp::Importer>();

    return shared_from_this();
}

std::shared_ptr<AssimpLoadPack> AssimpLoadPack::LoadAsync(std::wstring path, std::wstring name)
{
    auto pathString = std::to_string(path);
    this->asyncLoadThread = std::make_shared<std::jthread>(std::bind([](AssimpLoadPack* pack, std::stop_token token, std::string path)
    {
        pack->loadComplate.store(false);
        try
        {
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
                Debug::log << "Error : �� �ε� ���� : " << pack->importer->GetErrorString() << "\n";
                Debug::log << std::filesystem::current_path() << "\n";
            }
            Debug::log << "�� �ε� ���� : " << std::string(pack->scene->mName.data, pack->scene->mName.length) << "\n";
        }
        catch (...)
        {
            
        }
        pack->loadComplate.store(true);
    }, this, std::placeholders::_1, pathString));
    //this->asyncLoadThread->detach();
    this->name = name;
    return shared_from_this();
}

void ResourceManager::Init()
{
    
    
}

bool ResourceManager::IsLoading()
{
    bool loading = false;
    for (int i=0;i< _aiPackList.size();i++)
    {
        bool currentLoad = !_aiPackList[i]->loadComplate.load();
        loading |= currentLoad;
    }
    return loading;
}

bool ResourceManager::WaitAll()
{
    for (int i = 0; i < _aiPackList.size(); i++)
    {
        bool currentLoad = !_aiPackList[i]->loadComplate.load();
        if (currentLoad)
            _aiPackList[i]->asyncLoadThread->join();
    }
    return true;
}

std::shared_ptr<AssimpLoadPack> ResourceManager::LoadModel(std::wstring path, std::wstring name)
{
    auto pack = std::make_shared<AssimpLoadPack>()->Init()->LoadAsync(path, name);
    _aiPackList.push_back(pack);
    return pack;
}

void ResourceManager::LoadTexture(std::wstring path, std::wstring name)
{
    
}
