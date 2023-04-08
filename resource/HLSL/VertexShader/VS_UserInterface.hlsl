//----------------------------------------------
// Input
//----------------------------------------------
struct VertexShader_input
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
};

//----------------------------------------------
// Output
//----------------------------------------------
struct VertexShader_output
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

VertexShader_output VS(VertexShader_input input)
{
	VertexShader_output output = (VertexShader_output)0;
	output.p = float4(input.p, 1.0f);
	output.n = input.n;
	output.c = input.c;
	output.t = input.t;

	return output;
}