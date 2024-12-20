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

struct alignas(16) MainLightParams
{
    Matrix ViewMatrix = Matrix::Identity;
    Matrix ProjectionMatrix = Matrix::Identity;
    Matrix VPMatrix;
    Matrix InvertViewInvertMatrix;
    Matrix InvertProjectionMatrix;
    Matrix InvertVPMatrix;

    Vector4 lightPos; // worldPos
    Vector4 lightDirection; // worldDirction
    Vector4 lightUp;
    Vector4 lightFrustumData; // fovy aspect near far
    Vector4 lightScreenData; // screenx, screeny, offsetx, offsety
};

struct alignas(16) DrawRay
{
    Vector4 linePos[2];
    Vector4 color;
};

struct alignas(16) ReflectParams
{
    Matrix ReflectMatrix = Matrix::Identity;
};