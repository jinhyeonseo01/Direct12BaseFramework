


cbuffer CameraParams : register(b0)
{
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

cbuffer ObjectParams : register(b4)
{

};

//cbuffer BoneParams : register(b3)
//{
//    row_major matrix boneMatrix[256];
//};

cbuffer TransformParams : register(b2)
{
    row_major matrix WorldMatrix;
};

cbuffer DefaultMaterialParams : register(b1)
{
    float4 color3;
};

Texture2D test : register(t3);

SamplerState sampler_normal : register(s0);
SamplerState sampler_no_mip : register(s1);
SamplerState sampler_aniso_4 : register(s2);
SamplerState sampler_aniso_8 : register(s3);
SamplerState sampler_aniso_16 : register(s4);


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};


VS_OUT VS_Main(VS_IN input, uint vertexID : SV_VertexID)
{
    VS_OUT output = (VS_OUT) 0;

    // ���� ��ǥ ���
    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMatrix);
    output.worldPos = worldPos.xyz;
    //output.worldPos = input.pos;

    // �� �� ���� ��ȯ ����
    float4 viewPos = mul(worldPos, ViewMatrix);

    // ��� ��ȯ (���� �̵� ����)
    //output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMatrix));
    
    output.uv = input.uv.xy;
    output.color = input.color;
    output.pos = mul(viewPos, ProjectionMatrix);

    return output;
}


//[earlydepthstencil]
float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 color2 = input.color;

    float4 AlbedoColor = test.Sample(sampler_aniso_16, input.uv);

    return float4(color2, 1.0f) * pow(AlbedoColor, 1.0 / 2.2) * color3;
    //return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
}
