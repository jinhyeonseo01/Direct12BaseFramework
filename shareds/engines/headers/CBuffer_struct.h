

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
    Matrix viewMatrix = Matrix::Identity;
    Matrix projectionMatrix = Matrix::Identity;
};