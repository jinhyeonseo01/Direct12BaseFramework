#include "header_camera.hlsl"

cbuffer TransformParams : register(b2)
{
    row_major matrix WorldMatrix;
    row_major matrix NormalMatrix;
    unsigned int isSkinned;
};


Texture2D _BaseMap : register(t0);
SamplerState sampler_no_mip : register(s1);


struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
};

struct GS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN input) //, uint vertexID : SV_VertexID
{
    VS_OUT output = (VS_OUT)0;

    //float4 viewPos = mul(float4(input.pos, 1.0f), WorldMatrix);
    //viewPos = float4(input.pos.xy, 0.1f, 1.0f);
    output.pos = float4(input.pos, 1.0f); //mul(viewPos, ProjectionMatrix)
    return output;
}

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outStream)
{
    GS_OUT output = (GS_OUT)0;

    // 벡터 계산
    float3 basePos = input[0].pos.xyz;
    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, normalize(cameraPos.xyz - basePos)));
    float3 forward = cross(right, up); // 사용하지 않더라도 계산됨
    float size = 20;
    // 정점 1
    output.pos = mul(float4(basePos + up * size*2 + right * size, 1), VPMatrix);
    output.uv = float2(0, 0);
    outStream.Append(output);

    // 정점 2
    output.pos = mul(float4(basePos + up * size*2 - right * size, 1), VPMatrix);
    output.uv = float2(1, 0);
    outStream.Append(output);

    // 정점 3
    output.pos = mul(float4(basePos + right * size, 1), VPMatrix);
    output.uv = float2(0, 1);
    outStream.Append(output);

    // 삼각형 스트립 재시작
    outStream.RestartStrip();

    // 정점 4
    output.pos = mul(float4(basePos+ right * size, 1), VPMatrix);
    output.uv = float2(0, 1);
    outStream.Append(output);

    // 정점 5
    output.pos = mul(float4(basePos + up * size*2 - right * size, 1), VPMatrix);
    output.uv = float2(1, 0);
    outStream.Append(output);

    // 정점 6
    output.pos = mul(float4(basePos - right * size, 1), VPMatrix);
    output.uv = float2(1, 1);
    outStream.Append(output);
}


[earlydepthstencil]
float4 PS_Main(GS_OUT input) : SV_Target
{
    float4 AlbedoColor = _BaseMap.Sample(sampler_no_mip, input.uv);
    return pow(AlbedoColor, 1.0 / 2.2);
}
