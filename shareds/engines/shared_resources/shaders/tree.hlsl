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

Texture2D _BaseMap : register(t0);
SamplerState sampler_no_mip : register(s1);


struct VS_IN
{
    float3 pos : POSITION;
    float3 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN input) //, uint vertexID : SV_VertexID
{
    VS_OUT output = (VS_OUT)0;

    float4 viewPos = mul(float4(input.pos, 1.0f), WorldMatrix);
    //viewPos = float4(input.pos.xy, 0.1f, 1.0f);
    output.uv = input.uv.xy;
    output.pos = viewPos; //mul(viewPos, ProjectionMatrix)
    return output;
}

GS_OUT GS_Main(VS_OUT input) : SV_Target
{
    GS_OUT output = (GS_OUT)0;
    
    return output;
}


[earlydepthstencil]
float4 PS_Main(GS_OUT input) : SV_Target
{
    float4 AlbedoColor = _BaseMap.Sample(sampler_no_mip, input.uv);
    return pow(AlbedoColor, 1.0 / 2.2);
}
