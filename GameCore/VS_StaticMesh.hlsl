// VertexShader.txt, PixelShader.txt ���� �������� �ʰ� �ϳ��� ���Ͽ� ���ĳ��Ƶ� �ȴ�.


// HLSL (High Level Shader Language)
// �������̴��� ���� ���ۿ� �ִ� ��� ������ 1���� �۾��Ѵ�.
// ���� ��ġ(SV_POSITION : System Value Position ��������)�� �ݵ�� float4�� ��ȯ�ؾ� �Ѵ�.
// SV_POSITION(�ø�ƽ : �ǹ� ����) �ý��ۻ� �����Ǿ� ����. COLOR0 �� COLOR1 � ����.

// float3 p : POSITION ���� POSITION�� Sample.hpp->CreateVertexLayout->InputElementDescs �� ��� �̸��� ���ƾ���. 
// ���������� ���°� �ƴ϶� �׳� ���� �̸�ó�� ������ ���� �ҽ��ڵ�� �̸��� �����ϸ� ��.
// ���� ���̴��� ��ǲ ���̾ƿ��̶� �ٸ��� ���� ���� ����.
// SV_POSITION: ��� ������ ����� ���� ��ġ ��������

struct VertexShader_input
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
};

struct VertexShader_output
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
};

// ��� ���۴� �������� �����θ� �����.
// �������� ���� : float4(x, y, z, w)
// ��� ���۷� �Ѿ�� �ֵ��� ���� ������� ��.
cbuffer ConstantData : register(b0)
{
	matrix g_matWorld : packoffset(c0); // packoffset �� ������ �ʾƵ� �⺻������ c0���� �����ϳ� �ٿ��ִ°� ����
	matrix g_matView : packoffset(c4);
	matrix g_matProj : packoffset(c8);
	float4 g_light : packoffset(c12);
	//float  fTimer : packoffset(c12.z); // �̷������� �������Ϳ��� ������ ���� ��� �� ���� ����.
}

VertexShader_output VS(VertexShader_input _input)
{
	//VertexShader_output output;
	VertexShader_output output = (VertexShader_output)0;

	float4 vLocal = float4(_input.p, 1.0f);

	// ����! mul �Լ��� ����(dot)���� ó����.
	// vWorld.x = vLocal dot c0; // ��� ���� ������.
	// vWorld.y = vLocal dot c1;
	// vWorld.z = vLocal dot c2;
	// vWorld.w = vLocal dot c3;
	// ���� �������α׷� �ܿ��� ��ġ��ķ� ����� �����ų�, �Ʒ��� ���� ���̴����� �Ź� ��ġ��� ��ȯ�۾� �ʿ�.
	// matrix matTranspose = transpose(g_matWorld); // �Ź� ��ġ��� ��ȯ�۾��ϴ°� ��ȿ�����̹Ƿ� ������ �������α׷����� ��ġ�ؼ� ������ ��.

	float4 vWorld = mul(vLocal, g_matWorld);
	float4 vView = mul(vWorld, g_matView);
	float4 vProj = mul(vView, g_matProj);

	output.p = vProj;
	output.n = _input.n;
	output.c = _input.c;
	output.t = _input.t;

	//float3 vLight = float3(0, 0, 1);
	//float fdot = dot(_input.n, -vLight);
	//output.c = float4(fdot, fdot, fdot, 1.0); // �������� 0�̸� ���� ����. �ּ����� ���� ����Ͽ� ������ ��¦ ���̰� �ϴ°��� �����Ʈ �����̶� ��.

	return output;
}

//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos;
//}