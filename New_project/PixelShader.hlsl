struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    //float3 Colour : COLOR;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};
float4 PS(PS_INPUT input) : SV_Target0
{
    return float4(abs(normalize(input.Normal)) * 0.9f, 1.0);
    //return float4(input.Colour * abs(sin(time)), 1.0);
}
//cbuffer ConstantBufferStruct : register(b0)
//{
//    float time;
//};
