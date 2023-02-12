#pragma once
#include "Define.hpp"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")


// Direct X 2D�� ��� ���. DirectWrite ���� Ưȭ ���. 
// �ؽ�Ʈ, ��Ʈ�� ���õ� ����� �� ���⿡ ��� ����.
// Font Collention -> Font Family -> Font/Font Face ������ ������ �� ����.
// ���� ��� ������ -> �ҳ�Ÿ ���� -> �ҳ�Ÿ ���� ���踦 ������.
// gdi -> gdiplus -> directX 2D -> directX 3D ������ ������ ��. (���� ����� �����ؼ� ������ ����)
// 3D �׷��ȿ��� WinAPI ��� �Ұ�.

// �ظ��ϸ� DrawText�� ��� ���ϴ°� ����. ���������� �ֹ�. 
// ���� ���ӿ��� ��������� sprite�� �̿��Ͽ� ���� �̹����� ��¸� ���ִ� ��찡 ����.
// ä���̳� ��¿ �� ���� ���(���� �̹����� ���� ���) ����Ѵ�.
class DXWriter : public Singleton<DXWriter>
{
public:
	ID2D1Factory*			m_pd2dFactory = nullptr;
	IDWriteFactory*			m_pDWriteFactory = nullptr;
	ID2D1RenderTarget*		m_pd2dRenderTarget = nullptr; // ���� ������ �ʰ� 3D Device���� �����ͼ� ���.
	IDWriteTextFormat*		m_pTextFormat = nullptr; // ����(Text)�� �ٲ� �� ������ ��Ʈ�� ũ��� ���� �Ұ�, �����̳� ��ġ�� ���� ����.
	ID2D1SolidColorBrush*	m_pTextColor = nullptr;	// ���� ���� �귯��

	std::wstring			m_strDefault;

	IDWriteTextLayout*		m_pTextLayout = nullptr; // ����(Text)�� ���ٲٴ� ��� ��Ʈ�� ũ�� ���� ��� ���� ���� ����.

	bool bDrawLayout = false;

public:
	virtual bool initialize();
	virtual bool frame();
	virtual bool render();
	virtual bool release();
	virtual bool releaseDXResource();
	virtual bool createDXResource();

public:
	bool setBuffer(IDXGISurface* _dxgiSurface);
	void setString(std::wstring _str);
	bool draw(int _x, int _y, std::wstring _str, D2D1_COLOR_F _color = D2D1_COLOR_F({ 0.0f, 0.0f, 0.0f, 1.0f }), float _alpha = 1.0f);
};

