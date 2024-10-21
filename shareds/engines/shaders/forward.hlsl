/*
cbuffer TransformParams : register(b1)
{
    row_major matrix WorldMatrix;
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};


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
};

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);
Texture2D ShadowDepthTexture : register(t3);

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

*/


struct LightMateiral
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    int lightType;
    float3 specular;
    float dummy;
};

struct Light
{
    LightMateiral mateiral;
    float3 strength;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}


float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, LightMateiral mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float3 specular = mat.specular * pow(max(hdotn, 0.0f), mat.shininess);
    return mat.ambient + (mat.diffuse + specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, LightMateiral mat, float3 normal, float3 toEye)
{
    float3 lightVec = normalize(-L.direction);
    float ndotl = max(dot(normal, lightVec), 0.0f);
    float3 LightStrength = L.strength * ndotl;
    return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
}

float3 ComputePointLight(Light L, LightMateiral mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.position - pos;

    float d = length(lightVec);

    if (d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    
    else
    {
     
        lightVec /= d;
        
        float ndotl = saturate(dot(normal, lightVec));
        
        float3 LightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        LightStrength *= att;
        
        return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
    }
}

float3 ComputeSpotLight(Light L, LightMateiral mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.position - pos;

   
    float d = length(lightVec);

    if (d > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    
    else
    {
        lightVec /= d;
        
        float ndotl = saturate(dot(normal, lightVec));
        
        float3 LightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        LightStrength *= att;
        
        float spotFactor = pow(saturate(dot(-lightVec, L.direction)), L.spotPower);
        
        LightStrength *= spotFactor;
        
        return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
    }
}


#define MAX_LIGHTS 5 

cbuffer PixelConstantBuffer : register(b0)
{
    float3 g_eyeWorld;
    int g_lightCount;
    Light g_lights[MAX_LIGHTS];
    
    int useRimEffect;
    float3 rimColor;
    float rimPower;
    float rimStrength;
    float dummy1;
    float dummy2;
};

cbuffer TransformParams : register(b1)
{
    row_major matrix WorldMatrix;
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

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
};

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);
Texture2D ShadowDepthTexture : register(t3);

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
    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMatrix));
    
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 color = float3(0, 0, 0);
    
    float3 toEye = normalize(g_eyeWorld - input.worldPos);
    
    for (int i = 0; i < g_lightCount; ++i)
    {
   
        if (g_lights[i].mateiral.lightType == 0)
        {
            color += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, input.worldNormal, toEye);
            
            matrix shadowCameraVP = g_mat_0;
            
            float4 shadowClipPos = mul(float4(input.worldPos, 1.0f), shadowCameraVP);
            float depth = shadowClipPos.z / shadowClipPos.w;

            float2 uv = shadowClipPos.xy / shadowClipPos.w;
            uv.y = -uv.y;
            uv = uv * 0.5 + 0.5;

            float shadowDepth = ShadowDepthTexture.Sample(g_sam_0, uv).x;
            
            if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
            {
                color *= 0.2f;
            }
        }
        
        else if (g_lights[i].mateiral.lightType == 1)
        {
            color += ComputePointLight(g_lights[i], g_lights[i].mateiral, input.worldPos, input.worldNormal, toEye);
            
       
        }
        
        else if (g_lights[i].mateiral.lightType == 2)
        {
            color += ComputeSpotLight(g_lights[i], g_lights[i].mateiral, input.worldPos, input.worldNormal, toEye);
        }
          
    }
    
    
    
    return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
}
