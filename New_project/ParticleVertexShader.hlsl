// ShadowVertexShader_Instanced.hlsl

struct QuadVtx  //这个是用来画屏幕四边形的顶点结构体，顶点缓冲区的时候放0号槽位，从1号槽位读这个结构体的数据
{
    float3 corner;
    float2 uv;
};
struct ParticleInst  //这个是每个粒子的顶点结构体，将这个的顶点缓冲区绑订到1号槽位，每次GPU从1号槽位读到这个结构体的数据之后，就能知道每个粒子的位置和大小，相当于上面那个是按点读，下面按照instancing读
{
    float3 center;
    float size;
};
cbuffer ParticleBuffer : register(b0) //
{
    float4x4 VP;
    float3 cameraRight;
    float3 cameraUp;
};
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

cbuffer ShadowCB : register(b0)
{
    float4x4 VP;
};

cbuffer LightCB : register(b2)
{
    float3 lightDir;
    float pad0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    float4x4 W = float4x4(
        input.W0,
        input.W1,
        input.W2,
        input.W3
    );

    float4 worldPos = mul(float4(input.Pos, 1.0f), W);
    float planeY = 0.0f;
    float3 L = normalize(lightDir);

    if (abs(L.y) < 0.01f)
    {
        output.Pos = float4(0, 0, 0, 0);
        return output;
    }

    float t = (planeY - worldPos.y) / L.y;
    if (t < 0.0f)
    {
        output.Pos = float4(0, 0, -1000.0f, 1.0f);
        return output;
    }

    t = min(t, 500.0f);
    float3 shadowPos = worldPos.xyz + t * L;
    shadowPos.y = planeY + 0.02f;

    // 4️⃣ 投影到裁剪空间
    output.Pos = mul(float4(shadowPos, 1.0f), VP);
    return output;
}
