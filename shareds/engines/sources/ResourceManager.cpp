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
                aiProcess_MakeLeftHanded | // 왼손 좌표계로 변경
                aiProcess_FlipWindingOrder | //CW, CCW 바꾸는거임.
                aiProcess_FlipUVs | // 말그대로 uv의 y축을 뒤집음. 그리고 bitangent도 뒤집음.

                aiProcess_Triangulate | // 4각형 5각형을 3각형으로
                //aiProcess_GenSmoothNormals | // Normal이 없으면 Smmoth Normal 생성
                aiProcess_GenNormals | // Normal이 없으면 Normal 생성
                aiProcess_ImproveCacheLocality |// 삼각형 개선. 잘 되면 켜보기 캐시히트율을 위해 삼각형 재정렬함.
                aiProcess_GenUVCoords | // UV없으면 UV 계산하게[ 시키기
                aiProcess_CalcTangentSpace | // 탄젠트 계산
                //aiProcess_SplitLargeMeshes |// 매쉬가 너무 클때 쪼개는거 매쉬 클때 렌더링 유리.
                //aiProcess_Debone | 손실없이 뼈 제거. 걍 쓰지말자.
                //aiProcess_RemoveComponent | // (animations, materials, light sources, cameras, textures, vertex components 제거
                //aiProcess_PreTransformVertices | // root Node를 제외한 모든 하위 노드들 전부 평탄화. 계층 제거.
                //aiProcess_ValidateDataStructure | // 연결 유효성 검사
                //aiProcess_RemoveRedundantMaterials | // 중복이나 안쓰는거 제거
                aiProcess_FixInfacingNormals | //잘못 연결되서 고장난 노멀 재대로 수정
                //aiProcess_FindDegenerates | //삼각형에서 점이 겹쳐버리면 라인이나 점이 되버리는데, 이걸 Line이나 Point로 변환하는거임. 안쓰는게 나음.
                aiProcess_FindInvalidData | //유효하지 않는 데이터 감지후 수정, 법선 UV를 제거함. 이렇게 제거하고 나면 aiProcess_GenNormals같은게 새롭게 생성해줄거임. 애니메이션에서도 이점이 있다고함.
                //aiProcess_GenUVCoords  | //UV를 자체적으로 계산함. 모델링툴에서 생성하는걸 추천하고, UV가 없으면 새롭게 생성하는거임.
                //aiProcess_FindInstances | //너무 매쉬가 많을때 키나봄. 느리다는거같음. 같은 재질인 매쉬들을 하나로 합쳐버리는 기능인듯.
                //aiProcess_OptimizeMeshes |// 매쉬 를 줄여주는 최적화 옵션인듯. aiProcess_OptimizeGraph랑 같이 쓰는게 좋고, #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.랑 호환됨.
                //걍 안키는게 나을듯. 뭔가 밑에 옵션이랑 호환되는 모양인데, 밑에 옵션을 못씀.
                //aiProcess_OptimizeGraph |//필요없는 노드를 삭제함. 노드가 태그로 쓰일때 누락되는 문제가 잇나봄, 안키는게 나을듯. 계층구조가 손실된다고 함.
                //aiProcess_TransformUVCoords | //UV에 대해서 변환처리 한다고 하는거같음. 텍스쳐 이상해지면 꺼버리도록
                aiProcess_JoinIdenticalVertices | // 중복제거 후 인덱스 버퍼 기반으로 변환
                aiProcess_SortByPType // 폴리곤을 타입별로 재정렬함. aiProcess_Triangulate 쓰면 어차피 삼각형만 남아서 필요 없음. 일단 넣어~
            );
            if ((pack->scene == nullptr) || pack->scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || (pack->scene->mRootNode == nullptr))
            {
                Debug::log << "Error : 모델 로드 실패 : " << pack->importer->GetErrorString() << "\n";
                Debug::log << std::filesystem::current_path() << "\n";
            }
            Debug::log << "모델 로드 성공 : " << std::string(pack->scene->mName.data, pack->scene->mName.length) << "\n";
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
