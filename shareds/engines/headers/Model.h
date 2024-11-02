#pragma once

#include <stdafx.h>

#include "Bone.h"
#include "ResourceManager.h"

namespace dxe
{
    class ModelNode;

    class Model : public std::enable_shared_from_this<Model>
    {
    protected:
        int _boneAllocator = 0;

    public:
        std::wstring name = L"none";

        std::vector<std::shared_ptr<Mesh>> _meshList;
        std::vector<std::shared_ptr<Bone>> _boneList;
        std::vector<std::shared_ptr<ModelNode>> _nodeList;

        std::unordered_multimap<std::string, std::shared_ptr<Mesh>> _meshNameToMeshTable;
        std::unordered_map<std::string, std::shared_ptr<Bone>> _boneNameToBoneTable;
        std::unordered_map<std::string, std::shared_ptr<ModelNode>> _boneNameToNodeTable;
        std::unordered_map<std::string, std::shared_ptr<ModelNode>> _nodeNameToNodeTable;

        std::shared_ptr<ModelNode> rootNode = nullptr;
        std::shared_ptr<ModelNode> rootBoneNode = nullptr;

        int isSkinned = false;

        void Init(std::shared_ptr<AssimpPack> pack);
        void SetName(const std::wstring& name);


        std::shared_ptr<Mesh> AddMesh(const std::shared_ptr<Mesh>& mesh);
        std::shared_ptr<ModelNode> AddNode(const std::shared_ptr<ModelNode>& parentNode, aiNode* currentNode);
        std::shared_ptr<Bone> AddBone(std::shared_ptr<Bone> bone);

        std::vector<std::shared_ptr<Mesh>> GetMeshsByName(const std::string& name);
        std::shared_ptr<Mesh> GetMeshByNameSubIndex(const std::string& name, int index = 0);
        std::shared_ptr<ModelNode> GetNodeByName(const std::string& name);

        bool ContainsBone(const std::string& name);
        std::shared_ptr<Bone> GetBoneByName(const std::string& name);
        std::shared_ptr<Bone> GetBoneByID(int index);


    public:
        Model();
        virtual ~Model();

        void CreateGraphicResource();
    };

    class ModelNode : public std::enable_shared_from_this<ModelNode>
    {
    public:
        ModelNode();
        virtual ~ModelNode();

        std::string name;
        ModelNode* parent = nullptr;
        std::vector<ModelNode*> childs;
        Matrix transformMatrix = Matrix::Identity;

        Matrix calcLocalToWorldMatrix = Matrix::Identity;

        bool isBone = false;
        int boneID = -1;
        std::vector<std::string> meshNameList;

        void Init(const std::string& name, const Matrix& trans);
        void SetParent(ModelNode* parent);
        void SetBone(const std::shared_ptr<Bone>& bone);
        void AddMeshName(const std::string& meshName);
    };
}

