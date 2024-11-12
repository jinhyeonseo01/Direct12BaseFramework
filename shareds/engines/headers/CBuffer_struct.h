

struct TransformParams
{
    Matrix WorldMatrix = Matrix::Identity;
    Matrix NormalMatrix = Matrix::Identity;
    unsigned int isSkinned = false;
};
struct BoneParams
{
    Matrix boneMatrix[256];
};
struct DefaultMaterialParams
{
    Vector4 color;
};

struct CameraParams
{
    Matrix viewMatrix = Matrix::Identity;
    Matrix projectionMatrix = Matrix::Identity;
};