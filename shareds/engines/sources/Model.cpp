#include "stdafx.h"
#include "convert_assimp.h"
#include "Model.h"

#include "Bone.h"
#include "Mesh.h"


Model::Model()
{
    _meshList.reserve(64);

}

Model::~Model()
{
    _meshList.clear();
}

void Model::CreateGraphicResource()
{
    for (auto mesh : _meshList)
        mesh->CreateBothBuffer();
}

void Model::Init(std::shared_ptr<AssimpLoadPack> pack)
{
    _meshList.reserve(32);
    _boneList.reserve(256);
    _nodeList.reserve(256);

    _meshNameToMeshTable.reserve(32);
    _boneNameToBoneTable.reserve(256);
    _boneNameToNodeTable.reserve(256);
    _nodeNameToNodeTable.reserve(256);


    //this->SetName(std::string(pack->scene->mName.C_Str(), pack->scene->mName.length));

    if(pack->scene->HasMeshes())
    {

        _meshList.reserve(pack->scene->mNumMeshes);
        for(int i= 0;i< pack->scene->mNumMeshes;i++)
        {

            // ��ó�� ����

            auto& currentAIMesh = pack->scene->mMeshes[i];
            //mesh
            auto mesh = std::make_shared<Mesh>();
            std::vector<Vertex> vertexs;
            std::vector<uint32_t> indexs;

            vertexs.reserve(currentAIMesh->mNumVertices);
            indexs.reserve(currentAIMesh->mNumFaces * 3);

            //--------- Vertex ��� ---------
            Vertex vert;
            for(int j=0; j< currentAIMesh->mNumVertices; j++)
            {
                vert = {};
                vert.position = convert_assimp::Format(currentAIMesh->mVertices[j]);
                vert.normal = convert_assimp::Format(currentAIMesh->mNormals[j]);
                if (currentAIMesh->HasVertexColors(0))
                    vert.color = convert_assimp::Format(currentAIMesh->mColors[0][j]);
                if (currentAIMesh->HasTextureCoords(0))
                {
                    vert.tangent = convert_assimp::Format(currentAIMesh->mTangents[j]);
                    vert.bitangent = convert_assimp::Format(currentAIMesh->mBitangents[j]);
                    for (int k = 0; k < 8; k++)
                        if (currentAIMesh->HasTextureCoords(k))
                            vert.uvs[k] = convert_assimp::Format(currentAIMesh->mTextureCoords[k][j]);
                }
                vert.boneWeight = Vector4::Zero;
                vert.boneId = Vector4(-1, -1, -1, -1);
                vertexs.push_back(vert);
            }

            //--------- Index ��� ---------
            for (int j = 0; j < currentAIMesh->mNumFaces; j++)
            {
                auto& polygon = currentAIMesh->mFaces[3];
                if (polygon.mNumIndices == 3)
                {
                    indexs.push_back(polygon.mIndices[0]);
                    indexs.push_back(polygon.mIndices[1]);
                    indexs.push_back(polygon.mIndices[2]);
                }
            }

            //--------- Bone ����, Vertex�� ��� ---------
            for (int j = 0; j < currentAIMesh->mNumBones; j++)
            {
                auto currentAIBone = currentAIMesh->mBones[j];
                auto boneName = std::string(currentAIBone->mName.C_Str(), currentAIBone->mName.length);
                auto nodeName = boneName;
                auto boneNode = currentAIBone->mNode;
                if (boneNode != nullptr)
                    nodeName = std::string(boneNode->mName.C_Str(), boneNode->mName.length);

                str::trim(boneName);
                str::trim(nodeName);

                auto bone = GetBoneByName(boneName);
                if(bone == nullptr)
                {
                    bone = std::make_shared<Bone>();
                    bone->SetName(boneName, nodeName);
                    bone->SetOffsetTransform(convert_assimp::Format(currentAIBone->mOffsetMatrix));
                    AddBone(bone);
                }

                for (int k = 0; k < currentAIBone->mNumWeights; k++)
                {
                    auto& currentVertex = vertexs[currentAIBone->mWeights[k].mVertexId];
                    int findIndex = -1;
                    float* idArray = &currentVertex.boneId.x;
                    float* weightArray = &currentVertex.boneWeight.x;

                    // ���� �Ⱦ��� ID�� �ִ��� ����
                    for (int l = 3; l >= 0; --l)
                        if (Equals(idArray[l], -1))
                            findIndex = l;
                    // ���� ������ ����
                    if (findIndex == -1)
                    {
                        float minW = 1.1f;
                        for (int l = 0; l < 4; l++) {
                            if (weightArray[l] < minW)
                            {
                                minW = weightArray[l];
                                findIndex = l;
                            }
                        }
                    }
                    // ���� ������ ����.
                    if(findIndex != -1) {
                        reinterpret_cast<float*>(&currentVertex.boneId)[findIndex] = bone->boneId;
                        reinterpret_cast<float*>(&currentVertex.boneWeight)[findIndex] = currentAIBone->mWeights[k].mWeight;
                    }
                }
            }

            // ��ó��
            // ����ġ ����
            for(int vertexIndex = 0; vertexIndex < vertexs.size(); vertexIndex++)
            {
                auto& currentVertex = vertexs[vertexIndex];
                if(currentVertex.boneWeight.LengthSquared() > 1)
                    currentVertex.boneWeight.Normalize();
            }


            auto meshName = std::string(currentAIMesh->mName.C_Str(), currentAIMesh->mName.length);
            str::trim(meshName);
            mesh->SetName(meshName);
            mesh->Init(std::move(vertexs), std::move(indexs));
            mesh->SetBound(convert_assimp::Format(currentAIMesh->mAABB.mMin), convert_assimp::Format(currentAIMesh->mAABB.mMax));
            if (SimpleMath::Equals(Vector3(mesh->_bound.Extents).Length(), 0))
                mesh->CalculateBound();
            AddMesh(mesh);
        }
    }

    if (pack->scene->mRootNode != nullptr)
    {
        auto rootNode = AddNode(nullptr, pack->scene->mRootNode);
        int index = 0;
        for(int i=0;i<_boneList.size();i++)
        {
            auto& bone = _boneList[i];
            auto node = GetNodeByName(bone->nodeName);
            if(node != nullptr)
                node->SetBone(bone);
        }
    }

    //�� ����
    if(_nodeList.size() != 0)
        this->rootNode = _nodeList[0];
    if (_boneList.size() != 0)
        this->rootBoneNode = GetNodeByName(_boneList[0]->nodeName);
    isSkinned = _boneList.size() != 0;
}

void Model::SetName(const std::string& name)
{
    this->name = name;
}

std::shared_ptr<Mesh> Model::AddMesh(const std::shared_ptr<Mesh>& mesh)
{
    _meshList.push_back(mesh);
    _meshNameToMeshTable[mesh->name] = mesh;
    return mesh;
}

std::shared_ptr<ModelNode> Model::AddNode(const std::shared_ptr<ModelNode>& parentNode, aiNode* currentNode)
{
    if (currentNode == nullptr)
        return nullptr;
    auto name = std::string(currentNode->mName.C_Str(), currentNode->mName.length);
    //if (name == "RootNode")
        //return AddNode(parentNode, currentNode->mChildren[0]);
    
    auto trans = convert_assimp::Format(currentNode->mTransformation);
    auto thisNode = std::make_shared<ModelNode>();
    str::trim(name);
    thisNode->Init(name, trans);
    if(parentNode != nullptr)
        thisNode->SetParent(parentNode.get());
    for (int i = 0; i < currentNode->mNumMeshes; i++)
        thisNode->AddMeshName(_meshList[currentNode->mMeshes[i]]->name);

    _nodeList.push_back(thisNode);
    _nodeNameToNodeTable[name] = thisNode;

    for(int i = 0; i < currentNode->mNumChildren; i++)
    {
        AddNode(thisNode, currentNode->mChildren[i]);
    }
    return thisNode;
}

std::shared_ptr<ModelNode> Model::GetNodeByName(const std::string& name)
{
    if (_nodeNameToNodeTable.contains(name))
        return _nodeNameToNodeTable[name];
    return nullptr;
}

bool Model::ContainsBone(const std::string& name)
{
    return _boneNameToNodeTable.contains(name);
}

std::shared_ptr<Bone> Model::AddBone(std::shared_ptr<Bone> bone)
{
    if(!ContainsBone(bone->boneName))
    {
        _boneList.resize(_boneAllocator+1);
        bone->SetBoneID(_boneAllocator);

        _boneList[bone->boneId] = bone;
        _boneNameToBoneTable[bone->boneName] = bone;

        _boneAllocator++;
    }
    return bone;
}

std::shared_ptr<Bone> Model::GetBoneByName(const std::string& name)
{
    if (_boneNameToBoneTable.contains(name))
        return _boneNameToBoneTable[name];
    return nullptr;
}

std::shared_ptr<Bone> Model::GetBoneByID(int index)
{
    if (index >= 0 && index < _boneList.size())
        return _boneList[index];
    return nullptr;
}

std::shared_ptr<Mesh> Model::GetMeshByName(const std::string& name)
{
    if (_meshNameToMeshTable.contains(name))
        return _meshNameToMeshTable[name];
    return nullptr;
}



ModelNode::ModelNode()
{

}

ModelNode::~ModelNode()
{

}

void ModelNode::Init(const std::string& name, const Matrix& trans)
{
    childs.reserve(4);
    meshNameList.reserve(2);
    this->name = name;
    transformMatrix = trans;
}

void ModelNode::SetParent(ModelNode* parent)
{
    if (this->parent != nullptr)
    {
        auto iter = std::find(this->parent->childs.begin(), this->parent->childs.end(), this);
        if (iter != this->parent->childs.end())
            this->parent->childs.erase(iter);
    }
    this->parent = parent;
    if (this->parent != nullptr)
        this->parent->childs.push_back(this);
}

void ModelNode::AddMeshName(const std::string& meshName)
{
    meshNameList.push_back(meshName);
}

void ModelNode::SetBone(const std::shared_ptr<Bone>& bone)
{
    if (bone == nullptr)
    {
        this->boneID = -1;
        this->isBone = false;
        return;
    }
    this->boneID = bone->boneId;
    this->isBone = true;
}