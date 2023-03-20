#pragma once
#include "Actor.h"
#include "WidgetComponent.h"
#include <atlconv.h> // to_mw
#include <fstream>
#include <iostream>

class UI_Loader
{
public:
	void FileLoad(WidgetComponent* wc, std::wstring fileName);
	std::wstring to_mw(const std::string& _src);
	std::wstring FileReadToString(std::wstring readFileName);
	std::vector<std::wstring> SplitString(std::wstring inputStr, std::wstring delimiter);
	std::wstring GetSplitName(std::wstring fullpath);

public:
	std::wstring m_szFileContent = L"";
	std::vector<std::wstring> m_splitContent;
};