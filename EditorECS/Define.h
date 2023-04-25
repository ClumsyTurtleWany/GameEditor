#pragma once
#pragma comment(lib, "ECS.lib")
#pragma comment(lib, "EditorCore.lib")

#include "EditorCore\EditorCore.h"

static std::wstring to_mw(const std::string& _src)
{
	if (_src.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &_src[0], (int)_src.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &_src[0], (int)_src.size(), &wstrTo[0], size_needed);
	return wstrTo;
};

static std::string to_wm(const std::wstring& _src)
{
	if (_src.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &_src[0], (int)_src.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &_src[0], (int)_src.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;

};