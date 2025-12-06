struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Colour : COLOUR;
};
float4 PS(PS_INPUT input) : SV_Target0 //这个返回值会写入渲染目标（Render Target）的第 0 个附件
{
	return float4(input.Colour, 1.0);
}
