#pragma once

// WinAPI
#pragma comment(lib, "Ws2_32.lib") 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <windows.h> // windows.h 와 WinSock2 순서 중요함. WinSock2를 먼저 Include 하고 WinSock2를 해야 정상 작동이 됨.
#pragma comment(lib, "winmm.lib") // timeGetTime

#include <cstdlib> // srand
#include <ctime> // time


// Direct X
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3dcompiler.h> // D3DCompileFromFile
#pragma comment(lib, "d3dCompiler.lib")

#include <wrl.h> // Microsoft::WRL::ComPtr
using namespace Microsoft::WRL;

#include <DirectXCollision.h>

// Math
#include "DXTK/SimpleMath.h"


// STL
#include <vector>
#include <string>
#include <map>
#include <filesystem> // c++ 17 std
#include <memory>
//#include <fstream>

// Struct
#include "Vertex.h"
#include "IWData.h"
#include "CameraMatrix.h"
#include "TransformMatrix.h"
#include "LightData.h"
#include "BindPoseAnimationData.h"

// Design Pattern
template <class T>
class Singleton
{
private:
	static T* instance;

public:
	Singleton() {};
	virtual ~Singleton()
	{
		if (instance != nullptr)
		{
			delete instance;
			instance = nullptr;
		}
	}

public:
	static T* getInstance()
	{
		if (instance == nullptr)
		{
			instance = new T;
		}
		return instance;
	}
};

template <class T>
T* Singleton<T>::instance = nullptr;

static const double PI = 3.141592;