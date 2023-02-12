#pragma once
#include "DXDevice.hpp"
#include "Timer.hpp"
#include "Input.hpp"
#include "DXWriter.hpp"
#include "DXSamplerState.hpp"
#include "DXTextureManager.hpp"
#include "FMODSoundManager.hpp"
//#include "Object.hpp"
//#include "SpaceDivision.hpp"
//#include "QuadTree.hpp"
//#include "Octree.hpp"

class GameCore
{
private:
	HWND _hWnd;

public:
	GameCore(HWND hWnd) : _hWnd(hWnd) {};

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
	bool Run();
};