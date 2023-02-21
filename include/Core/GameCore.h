#pragma once
#include "DXDevice.hpp"
#include "Timer.hpp"
#include "Input.hpp"
#include "DXSamplerState.hpp"
#include "DXTextureManager.hpp"
#include "FMODSoundManager.hpp"
//#include "Object.hpp"
//#include "SpaceDivision.hpp"
//#include "QuadTree.hpp"
//#include "Octree.hpp"
#include "DXRenderTarget.hpp"

class GameCore
{
protected:
	HWND _hWnd;
	DXDevice Device;
	//DXRenderTarget RenderTarget;

public:
	GameCore() {};

public:
	virtual bool Initialize() { return true; };
	virtual bool Frame() { return true; };
	virtual bool Render() { return true; };
	virtual bool Release() { return true; };

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