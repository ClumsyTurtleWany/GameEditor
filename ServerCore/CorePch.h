#pragma once

#include"Types.h"
#include"CoreGlobal.h"
#include"CoreMacro.h"
#include"CoreTLS.h"
#include"Container.h"

#include<Windows.h>
#include <iostream>
#include<string>

//network
#include<WinSock2.h>
#include<MSWSock.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include"Lock.h"
#include"ObjectPool.h"
#include"TypeCast.h"
#include"Memory.h"
#include"SendBuffer.h"
#include"Session.h"
#include"ThreadManager.h"

using namespace std;
