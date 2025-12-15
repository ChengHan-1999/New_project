
// Vertex Shader
struct VS_INPUT
{
	float3 Pos : POSITION;
    //float3 Colour : COLOR;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
    float4x4 World : WORLD;
};
// Pixel Shader
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
    //float3 Colour : COLOR;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;

};
cbuffer staticMeshBuffer : register(b0) //
{
    float4x4 VP;
};
cbuffer BoneBuffer : register(b1) 
{
    float4x4 bones[256];
};
// Passthrough Shader
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
    //output.Pos = float4(input.Pos, 1.0f);
    output.Pos = mul(float4(input.Pos, 1.0f), input.World); 
    output.Pos = mul(output.Pos, VP);
    output.Normal = mul(input.Normal, (float3x3) input.World);
    output.Tangent = mul(input.Tangent, (float3x3) input.World);
    output.TexCoords = input.TexCoords;
	//output.Colour = input.Colour;
    //output.Pos = mul(W,float4(input.Pos, 1.0f));
    //output.Pos = mul(VP,output.Pos);
    //output.Normal = mul((float3x3) W, input.Normal);
    //output.Tangent = mul((float3x3) W, input.Tangent);  //
	return output;
}