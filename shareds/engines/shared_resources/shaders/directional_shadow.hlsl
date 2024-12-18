#include "header_camera.hlsl"

cbuffer ObjectParams : register(b4)
{

};

cbuffer BoneParams : register(b3)
{
    row_major matrix bone[256];
};

cbuffer TransformParams : register(b2)
{
    row_major matrix WorldMatrix;
    row_major matrix NormalMatrix;
    unsigned int isSkinned;
};

struct VS_IN
{
    float3 pos : POSITION;
    float4 boneIDs : BONEIDs;
    float4 boneWs : BONEWs;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
};


VS_OUT VS_Main(VS_IN input) //, uint vertexID : SV_VertexID
{
    VS_OUT output;

    float4 worldPos = float4(input.pos, 1.0f);

    if (isSkinned != 0)
    {
        matrix boneMatrix = float4x4(0, 0, 0, 0,
                                     0, 0, 0, 0,
                                     0, 0, 0, 0,
                                     0, 0, 0, 0);

        if (input.boneWs.x > 0.0001f)
            boneMatrix += bone[(int)input.boneIDs.x] * input.boneWs.x;
        if (input.boneWs.y > 0.0001f)
            boneMatrix += bone[(int)input.boneIDs.y] * input.boneWs.y;
        if (input.boneWs.z > 0.0001f)
            boneMatrix += bone[(int)input.boneIDs.z] * input.boneWs.z;
        if (input.boneWs.w > 0.0001f)
            boneMatrix += bone[(int)input.boneIDs.w] * input.boneWs.w;

        worldPos = mul(worldPos, boneMatrix);
    }
    else
    {
        worldPos = mul(worldPos, WorldMatrix);

    }

    //float4 viewPos = mul(worldPos, ViewMatrix);

    // 노멀 변환 (평행 이동 제외)
    //output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMatrix));
    //output.pos = mul(viewPos, ProjectionMatrix);
    output.pos = mul(worldPos, VPMatrix);
    return output;
}


float4 PS_Main(VS_OUT input) : SV_Target
{
    //return float4(input.pos.z,0,0,0);
    return float4(input.pos.z,0,0,0);
}
