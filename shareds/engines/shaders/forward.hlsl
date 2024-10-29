

cbuffer TransformParams : register(b2)
{
    row_major matrix WorldMatrix;
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

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

Texture2D test : register(t3);
SamplerState g_sam_0 : register(s0);

VS_OUT VS_Main(VS_IN input, uint vertexID : SV_VertexID)
{
    VS_OUT output = (VS_OUT) 0;

    // 월드 좌표 계산
    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMatrix);
    output.worldPos = worldPos.xyz;
    //output.worldPos = input.pos;

    // 뷰 및 투영 변환 적용
    //float4 viewPos = mul(worldPos, ViewMatrix);
    //output.pos = mul(viewPos, ProjectionMatrix);

    // 노멀 변환 (평행 이동 제외)
    //output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMatrix));
    
    output.uv = input.uv.xy;
    output.color = input.color;
    output.pos = worldPos;

    return output;
}


//[earlydepthstencil]
float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 color = input.color;

    float4 AlbedoColor = test.Sample(g_sam_0, input.uv);

    return float4(color, 1.0f) * pow(AlbedoColor, 1.0 / 2.2);
    //return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
}
