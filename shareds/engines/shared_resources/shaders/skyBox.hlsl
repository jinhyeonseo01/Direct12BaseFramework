cbuffer TransformParams : register(b2)
{
    row_major matrix WorldMatrix;
    row_major matrix NormalMatrix;
    unsigned int isSkinned;
};

cbuffer CameraParams : register(b0)
{
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

Texture2D skyTexture : register(t0);
SamplerState sampler_no_mip : register(s1);


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};


VS_OUT VS_Main(VS_IN input) //, uint vertexID : SV_VertexID
{
    VS_OUT output = (VS_OUT)0;
    float4 pos = float4(input.pos, 1.0f); //  + input.normal * -0.01
    float4 viewPos = mul(mul(pos, WorldMatrix), ViewMatrix);
    output.uv = input.uv.xy;
    output.pos = mul(viewPos, ProjectionMatrix);
    return output;
}


//[earlydepthstencil]
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 AlbedoColor = skyTexture.Sample(sampler_no_mip, input.uv);
    return pow(AlbedoColor, 1.0 / 2.2);
}
