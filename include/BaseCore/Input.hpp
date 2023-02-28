#pragma once
#include "Define.h"
#define KEY_COUNT (int)256

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

public:
	virtual bool initialize();
	virtual bool frame();
	virtual bool render();
	virtual bool release();

public:
	KeyState getKey(DWORD _key);
	void setWndHandle(HWND _hWnd);
	bool isPressedAnyKey();
	
};
