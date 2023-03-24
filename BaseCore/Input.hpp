#pragma once
#include "Define.h"
#define KEY_COUNT (int)256

//Wheel �Է½� ���콺 ��ġ ����
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

// ���� 9���� Direct Input �� ������ ���⼭�� window �Լ� ��� ����. ���߿� �ٲ� �� ��������?
class Input : public Singleton<Input>
{
private:
	HWND		hWnd;

public:
	KeyState dwKeyState[KEY_COUNT]; // mouse ��ư + keyboard ��ư
	POINT m_ptPos; // mouse position
	POINT m_ptPrevPos;
	POINT m_ptOffset;

	////////////////////////////////////////////
	// �� ������ �ּ�ȭ ����
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
