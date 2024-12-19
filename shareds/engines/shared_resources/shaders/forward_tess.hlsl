#include "header_camera.hlsl"
#include "header_light.hlsl"

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

cbuffer DefaultMaterialParams : register(b1)
{
    float4 _color;
};

Texture2D _BaseMap : register(t1);

SamplerState sampler_normal : register(s0);
SamplerState sampler_no_mip : register(s1);
SamplerState sampler_aniso_4 : register(s2);
SamplerState sampler_aniso_8 : register(s3);
SamplerState sampler_aniso_16 : register(s4);


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 uv : TEXCOORD0;
    float4 color : COLOR;
    float4 boneIDs : BONEIDs;
    float4 boneWs : BONEWs;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;

    float4 shadowUV : POSITION2;
};

struct HS_OUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;

    float4 shadowUV : POSITION2;
};
struct DS_OUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;

    float4 shadowUV : POSITION2;
};

VS_OUT VS_Main(VS_IN input) //, uint vertexID : SV_VertexID
{
    VS_OUT output = (VS_OUT)0;

    output.worldPos = float4(input.pos, 1.0f);

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

        output.worldPos = mul(output.worldPos, boneMatrix);
    }
    else
    {
        output.worldPos = mul(output.worldPos, WorldMatrix);

    }

    float4 viewPos = mul(output.worldPos, ViewMatrix);

    // 노멀 변환 (평행 이동 제외)
    //output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMatrix));
    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), NormalMatrix).xyz);
    output.uv = input.uv.xy;
    output.color = input.color;
    //output.pos = mul(viewPos, ProjectionMatrix);
    output.pos = mul(output.worldPos, VPMatrix);
    output.shadowUV = mul(output.worldPos, mul(lightVPMatrix, matrix(
    0.5,0,0,0,
    0,-0.5,0,0,
    0,0,1,0,
    0.5,0.5,0,1
    )));
    return output;
}


struct PatchTess
{
    float edgeTess[3] :  SV_TessFactor;
    float insideTess[1] :  SV_InsideTessFactor;
};


PatchTess ConstantHS(InputPatch<VS_OUT, 3> input, int patchID : SV_PrimitiveID)
{
    PatchTess output = (PatchTess)0.f;

    float3 patchCenter = ((input[0].worldPos + input[1].worldPos + input[2].worldPos) / 3.0f).xyz;

    // 카메라와 패치 중심 간의 거리 계산
    float distance2 = distance(patchCenter, cameraPos.xyz);

    // 거리에 따른 테셀레이션 팩터 계산
    // 예: 거리가 가까울수록 높은 팩터, 멀수록 낮은 팩터
    float tessFactor = saturate(1.0f / (distance2 * 0.05f)); // 거리 스케일 조정 필요

    // 최소 및 최대 테셀레이션 팩터 설정
    tessFactor = lerp(1.0f, 8.0f, tessFactor); // 예: 최소 2, 최대 8

    output.edgeTess[0] = tessFactor;
    output.edgeTess[1] = tessFactor;
    output.edgeTess[2] = tessFactor;
    output.insideTess[0] = tessFactor;

    return output;
}

[domain("tri")] //패치의 종류다. 삼각형이니 tri(tri, quad, isoline)
[partitioning("integer")] // 테셀레이션 세분화 모드.(integer, fractional_even, fractional_odd)
[outputtopology("triangle_cw")] // 새로 만들어진 삼각형들의 감기는 순서.(cw : 시계방향, ccw : 반시계, line : 선 테셀레이션)
[outputcontrolpoints(3)] // 입력된 컨트롤 포인트 갯수
[patchconstantfunc("ConstantHS")]
HS_OUT HS_Main(InputPatch<VS_OUT, 3> input, int vertexIdx : SV_OutputControlPointID, int patchID : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT)0.f;

    output.pos = input[vertexIdx].pos;
    output.uv = input[vertexIdx].uv;
    output.shadowUV = input[vertexIdx].shadowUV;
    output.color = input[vertexIdx].color;
    output.worldNormal = input[vertexIdx].worldNormal;
    output.worldPos = input[vertexIdx].worldPos;

    return output;
}

[domain("tri")]
DS_OUT DS_Main(const OutputPatch<HS_OUT, 3> input, float3 location : SV_DomainLocation, PatchTess patch)
{
    DS_OUT output = (DS_OUT)0.f;

    float2 uv = input[0].uv * location[0] + input[1].uv * location[1] + input[2].uv * location[2];
    float4 shadowUV = input[0].shadowUV * location[0] + input[1].shadowUV * location[1] + input[2].shadowUV * location[2];
    float4 color = input[0].color * location[0] + input[1].color * location[1] + input[2].color * location[2];
    float4 pos = input[0].pos * location[0] + input[1].pos * location[1] + input[2].pos * location[2];
    float4 worldPos = input[0].worldPos * location[0] + input[1].worldPos * location[1] + input[2].worldPos * location[2];
    float3 worldNormal = input[0].worldNormal * location[0] + input[1].worldNormal * location[1] + input[2].worldNormal * location[2];

    output.uv = uv;
    output.color = color;
    output.pos = pos;
    output.worldNormal = worldNormal;
    output.worldPos = worldPos;
    output.shadowUV = shadowUV;

    return output;
}



//[earlydepthstencil]
float4 PS_Main(DS_OUT input) : SV_Target
{
    float4 AlbedoColor = _BaseMap.Sample(sampler_aniso_16, input.uv);
    float shadowAtt = (_MainLightShadowMap.Sample(sampler_no_mip, input.shadowUV.xy / input.shadowUV.ww).r + 0.0005f - input.shadowUV.z)<0?0:1;

    shadowAtt = shadowAtt * saturate(dot(-lightDirection, input.worldNormal))*0.7+0.3;

    return AlbedoColor * shadowAtt;
    //float4 AlbedoColor = test.Sample(sampler_aniso_4, input.uv);
    //float4 AlbedoColor = test.Sample(sampler_no_mip, input.uv);
    //return input.color;
    return pow(AlbedoColor, 1.0 / 1) * _color * (saturate(dot(input.worldNormal, normalize(float3(0.7, 0.8, -1)))) * 0.5
        + 0.5f); //* _color pow(AlbedoColor, 1.0 / 1) * 
    //return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
}
