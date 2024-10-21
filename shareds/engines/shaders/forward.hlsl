
cbuffer TransformParams : register(b1)
{
    row_major matrix WorldMatrix;
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

/*
cbuffer MATERIAL_PARAMS : register(b3)
{
    int int_0;
    int int_1;
    int int_2;
    int int_3;
    
    float hit;
    float float_1;
    float float_2;
    float float_3;
    
    row_major float4x4 g_mat_0;
};*/

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);
//Texture2D ShadowDepthTexture : register(t3);

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // 월드 좌표 계산
    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMatrix);
    output.worldPos = worldPos.xyz;

    // 뷰 및 투영 변환 적용
    float4 viewPos = mul(worldPos, ViewMatrix);
    output.pos = mul(viewPos, ProjectionMatrix);

    // 노멀 변환 (평행 이동 제외)
    output.worldNormal = mul((float3x3) WorldMatrix, input.normal);
    
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 color = float3(0, 0, 0);
    
    float3 worldnormal = normalize(input.worldNormal);

    
    return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
}
