#pragma once

struct alignas(16) TransformParams
{
    Matrix WorldMatrix = Matrix::Identity;
    Matrix NormalMatrix = Matrix::Identity;
    unsigned int isSkinned = false;
};

struct alignas(16) BoneParams
{
    Matrix boneMatrix[256];
};

struct alignas(16) DefaultMaterialParams
{
    Vector4 color;
};

struct alignas(16) CameraParams
{
    Matrix ViewMatrix = Matrix::Identity;
    Matrix ProjectionMatrix = Matrix::Identity;
    Matrix VPMatrix;
    Matrix InvertViewMatrix;
    Matrix InvertProjectionMatrix;
    Matrix InvertVPMatrix;

    Vector4 cameraPos; // worldPos
    Vector4 cameraDirection; // worldDirction
    Vector4 cameraUp; // world Up Dirction
    Vector4 cameraFrustumData; // x:fovy, y:aspect, z:near, w:far
    Vector4 cameraScreenData; // x:screenx, y:screeny, z:offsetx, w:offsety
};
