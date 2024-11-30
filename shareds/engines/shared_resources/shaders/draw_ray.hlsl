#include "header_camera.hlsl"

cbuffer DrawRay : register(b1)
{
    float4 linePos[2];
    float4 _Color;
};


struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
};


VS_OUT VS_Main(VS_IN input, uint vertexID : SV_VertexID) //, uint vertexID : SV_VertexID
{
    VS_OUT output = (VS_OUT)0;
    output.pos = mul(float4(linePos[vertexID].xyz, 1.0f), VPMatrix);
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return _Color;
}
