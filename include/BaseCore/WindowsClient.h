#pragma once
#include "Define.h"

class WindowsClient
{
private:
	HINSTANCE	hInstance;
	HWND		hWnd;
	RECT		ClientRect;

protected:
	bool		InitInstance(const WCHAR* title, LONG width, LONG height);
	ATOM		MyRegisterClass();

public:
	bool		Create(HINSTANCE instance, const WCHAR* title, LONG width, LONG height);

public:
	HWND		GetHandle();

public:
	virtual bool Run();
};