


cbuffer CameraParams : register(b0)
{
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

cbuffer ObjectParams : register(b4)
{

};

cbuffer TransformParams : register(b2)
{
    row_major matrix WorldMatrix;
    row_major matrix NormalMatrix;
    unsigned int isSkinned;
};


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
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};


VS_OUT VS_Main(VS_IN input) //, uint vertexID : SV_VertexID
{
    VS_OUT output = (VS_OUT) 0;

    output.worldPos = float4(input.pos, 1.0f);

    output.worldPos = mul(output.worldPos, WorldMatrix);

    float4 viewPos = mul(output.worldPos, ViewMatrix);

    // 노멀 변환 (평행 이동 제외)
    
    output.uv = input.uv.xy;
    output.color = input.color;
    output.pos = mul(viewPos, ProjectionMatrix);
    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), NormalMatrix));
    return output;
}


//[earlydepthstencil]
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color2 = input.color;
    return 0.5f * (dot(input.worldNormal, normalize(float3(1,1,-1)))*0.5+0.5f);
    //return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
}
