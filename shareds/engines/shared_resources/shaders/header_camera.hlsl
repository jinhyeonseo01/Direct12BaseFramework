#ifndef CAMERA_HEADER_HLSL
#define CAMERA_HEADER_HLSL

cbuffer CameraParams : register(b0)
{
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
    row_major matrix VPMatrix;
    row_major matrix InvertViewInvertMatrix;
    row_major matrix InvertProjectionMatrix;
    row_major matrix InvertVPMatrix;

    float4 cameraPos; // worldPos
    float4 cameraDirection; // worldDirction
    float4 cameraUp;
    float4 cameraFrustumData; // fovy aspect near far
    float4 cameraScreenData; // screenx, screeny, offsetx, offsety
};

#endif