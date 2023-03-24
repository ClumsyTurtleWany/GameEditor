#pragma once
#include "Define.h"
#define KEY_COUNT (int)256

//Wheel 입력시 마우스 위치 추출
#define EXTRACT_LPARAM_X(lParam) ((LONG)(lParam & 0xffff))
#define EXTRACT_LPARAM_Y(lParam) ((LONG)((lParam >> 16) & 0xffff))

struct MOUSE_WHEEL_STATE
{
	bool	bMoveWheel;
	int		iWheelDt;
	POINT	pt;
	DWORD	dwExtraKeyInput;
};

enum class KeyState
{
	Free,
	Down,
	Up,
	Hold
};

// 교재 9강에 Direct Input 이 있으나 여기서는 window 함수 사용 했음. 나중에 바꿀 수 있을지도?
class Input : public Singleton<Input>
{
private:
	HWND		hWnd;

public:
	KeyState dwKeyState[KEY_COUNT]; // mouse 버튼 + keyboard 버튼
	POINT m_ptPos; // mouse position
	POINT m_ptPrevPos;
	POINT m_ptOffset;

	////////////////////////////////////////////
	// 휠 정보와 최소화 정보
	////////////////////////////////////////////
	MOUSE_WHEEL_STATE	m_wheelState;
	bool				m_bMinimized;

public:
	virtual bool initialize();
	virtual bool frame();
	virtual bool render();
	virtual bool release();

public:
	KeyState getKey(DWORD _key);
	void setWndHandle(HWND _hWnd);
	bool isPressedAnyKey();

	void cleanUpWheelState();
};
