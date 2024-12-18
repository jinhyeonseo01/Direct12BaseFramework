#ifndef LIGHT_HEADER_HLSL
#define LIGHT_HEADER_HLSL

cbuffer MainLightParams : register(b7)
{
    row_major matrix lightViewMatrix;
    row_major matrix lightProjectionMatrix;
    row_major matrix lightVPMatrix;
    row_major matrix lightInvertViewInvertMatrix;
    row_major matrix lightInvertProjectionMatrix;
    row_major matrix lightInvertVPMatrix;

    float4 lightPos; // worldPos
    float4 lightDirection; // worldDirction
    float4 lightUp;
    float4 lightFrustumData; // fovy aspect near far
    float4 lightScreenData; // screenx, screeny, offsetx, offsety
};

Texture2D _MainLightShadowMap : register(t7);

#endif