


cbuffer CameraParams : register(b0)
{
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

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
    unsigned int isSkinned;
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
    float3 uv : TEXCOORD0;
    float4 color : COLOR;
    float4 boneIDs : BONEIDs;
    float4 boneWs : BONEWs;
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

    if(isSkinned != 0)
    {
        matrix boneMatrix = float4x4(0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 0);
        
        if(input.boneWs.x > 0.0001f)
            boneMatrix += bone[(int)input.boneIDs.x] * input.boneWs.x;
        if(input.boneWs.y > 0.0001f)
            boneMatrix += bone[(int)input.boneIDs.y] * input.boneWs.y;
        if(input.boneWs.z > 0.0001f)
            boneMatrix += bone[(int)input.boneIDs.z] * input.boneWs.z;
        if(input.boneWs.w > 0.0001f)
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
    
    output.uv = input.uv.xy;
    output.color = input.color;
    output.pos = mul(viewPos, ProjectionMatrix);
    return output;
}


//[earlydepthstencil]
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color2 = input.color;

    float4 AlbedoColor = test.Sample(sampler_aniso_16, input.uv);
    //float4 AlbedoColor = test.Sample(sampler_aniso_4, input.uv);
    //float4 AlbedoColor = test.Sample(sampler_no_mip, input.uv);
    //return input.color;
    return pow(AlbedoColor, 1.0 / 1.0);
    //return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
}
