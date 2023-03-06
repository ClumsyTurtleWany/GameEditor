#pragma once
#include "DXDevice.hpp"
#include "DXShaderManager.h"
#include "Timer.hpp"
#include "Input.hpp"
#include "DXSamplerState.hpp"
#include "DXTextureManager.hpp"
#include "FMODSoundManager.hpp"

class BaseCore
{
protected:
	HWND _hWnd;

public:
	BaseCore();
	virtual ~BaseCore();

protected:
	virtual bool Initialize();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();

public:
	bool CoreInitialize();
	bool CoreFrame();
	bool CoreRender();
	bool CoreRelease();
	bool PreRender();
	bool PostRender();

public:
	bool ResizeDevice(int x, int y, int width, int height);

public:
	bool Run();

public:
	void SetWindowHandle(HWND hWnd);
};