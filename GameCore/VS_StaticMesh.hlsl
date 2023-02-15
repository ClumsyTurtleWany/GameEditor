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
	float4 c : COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
	float4 lightColor : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
	float3 light : TEXCOORD3;
};

//struct VertexShader_output
//{
//	float4 p : SV_POSITION;
//	float3 n : NORMAL;
//	float4 c : COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
//	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
//	float4x4 world : TEXCOORD1;
//	float4x4 view : TEXCOORD5;
//	float4x4 proj : TEXCOORD9;
//};

//----------------------------------------------
// Constant Buffer
//----------------------------------------------
// ��� ���۴� �������� �����θ� �����.
// �������� ���� : float4(x, y, z, w)
// ��� ���۷� �Ѿ�� �ֵ��� ���� ������� ��.
// packoffset �� ������ �ʾƵ� �⺻������ c0���� �����ϳ� �ٿ��ִ°� ����
cbuffer TransformData : register(b0)
{
	matrix g_WorldTransform : packoffset(c0); 
}

cbuffer CameraMatrixData : register(b1)
{
	matrix g_View : packoffset(c0);
	matrix g_Projection : packoffset(c4);
}

//cbuffer LightData : register(b2)
//{
//	float4 g_LightDirection : packoffset(c12);
//	float4 g_LightColor : packoffset(c13);
//}

VertexShader_output VS(VertexShader_input input)
{
	//VertexShader_output output;
	VertexShader_output output = (VertexShader_output)0;

	float4 vLocal = float4(input.p, 1.0f);

	// ����! mul �Լ��� ����(dot)���� ó����.
	// vWorld.x = vLocal dot c0; // ��� ���� ������.
	// vWorld.y = vLocal dot c1;
	// vWorld.z = vLocal dot c2;
	// vWorld.w = vLocal dot c3;
	// ���� �������α׷� �ܿ��� ��ġ��ķ� ����� �����ų�, �Ʒ��� ���� ���̴����� �Ź� ��ġ��� ��ȯ�۾� �ʿ�.
	// matrix matTranspose = transpose(g_matWorld); // �Ź� ��ġ��� ��ȯ�۾��ϴ°� ��ȿ�����̹Ƿ� ������ �������α׷����� ��ġ�ؼ� ������ ��.

	float4 vWorld = mul(vLocal, g_WorldTransform);
	float4 vView = mul(vWorld, g_View);
	float4 vProj = mul(vView, g_Projection);

	output.p = vProj;
	output.n = input.n;
	output.c = input.c;
	output.t = input.t;

	output.vWorld = vWorld;
	
	float3 vLight = float3(1.0f, 1.0f, 1.0f);//g_LightColor.xyz; // �̷������� ��� ����
	float fdot = max(0.3f, dot(output.n, -vLight));// �������� 0�̸� ���� ����. �ּ����� ���� ����Ͽ� ������ ��¦ ���̰� �ϴ°��� �����Ʈ �����̶� ��.
	output.lightColor = float4(fdot, fdot, fdot, 1.0f);
	output.light = vLight;

	//output.world = g_WorldTransform;
	//output.view = g_View;
	//output.proj = g_Projection;

	return output;
}