struct VS_INPUT   //这是顶点着色器的输入结构体，必须和VB后的数据格式对应，所以必要要定义layout的原因就是为了让编译器知道数据的格式和布局
{
	float4 Pos : POSITION; //语义，它就是顶点布局和HLSL输入连接起来的桥梁，确定了数据的含义
	float4 Colour : COLOUR;
};
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Colour : COLOUR;
};
PS_INPUT VS(VS_INPUT input)  //这是vs的入口函数，输出是PS_INPUT结构体，不是顶点着色器
{
	PS_INPUT output;
	output.Pos = input.Pos;  //这里之所以会原样转发是因为必须确保我前面构建的顶点本身就是屏幕空间坐标，所以不再需要做矩阵变化
	output.Colour = input.Colour;
	return output;
}

	