#pragma once
#include "Define.hpp"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")


// Direct X 2D의 기능 사용. DirectWrite 글자 특화 기능. 
// 텍스트, 폰트와 관련된 기능은 다 여기에 들어 있음.
// Font Collention -> Font Family -> Font/Font Face 순으로 나뉘어 져 있음.
// 예를 들면 현대차 -> 소나타 종류 -> 소나타 같은 관계를 형성함.
// gdi -> gdiplus -> directX 2D -> directX 3D 순서로 나오게 됨. (기존 기능은 불편해서 개선에 개선)
// 3D 그래픽에는 WinAPI 사용 불가.

// 왠만하면 DrawText는 사용 안하는게 좋다. 성능저하의 주범. 
// 보통 게임에서 글자출력은 sprite를 이용하여 글자 이미지로 출력만 해주는 경우가 많음.
// 채팅이나 어쩔 수 없는 경우(글자 이미지가 없는 경우) 사용한다.
class DXWriter : public Singleton<DXWriter>
{
public:
	ID2D1Factory*			m_pd2dFactory = nullptr;
	IDWriteFactory*			m_pDWriteFactory = nullptr;
	ID2D1RenderTarget*		m_pd2dRenderTarget = nullptr; // 직접 만들지 않고 3D Device에서 가져와서 사용.
	IDWriteTextFormat*		m_pTextFormat = nullptr; // 글자(Text)를 바꿀 수 있지만 폰트와 크기는 변경 불가, 색상이나 위치는 변경 가능.
	ID2D1SolidColorBrush*	m_pTextColor = nullptr;	// 색상 관련 브러쉬

	std::wstring			m_strDefault;

	IDWriteTextLayout*		m_pTextLayout = nullptr; // 글자(Text)를 못바꾸는 대신 폰트나 크기 색상 등등 전부 변경 가능.

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

