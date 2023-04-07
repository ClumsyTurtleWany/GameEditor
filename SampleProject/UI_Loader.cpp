#include "UI_Loader.h"
#include <tchar.h>

std::wstring UI_Loader::to_mw(const std::string& _src)
{
	USES_CONVERSION;
	return std::wstring(A2W(_src.c_str()));
}

std::wstring UI_Loader::FileReadToString(std::wstring readFileName)
{
	std::ifstream file(readFileName);
	if (!file.is_open()) {
		// handle file open error
	}

	std::string fileContents;
	std::string line;
	while (std::getline(file, line)) {
		fileContents += line + "\n";
	}
	file.close();

	return to_mw(fileContents);
}

std::vector<std::wstring> UI_Loader::SplitString(std::wstring inputStr, std::wstring delimiter)
{
	std::vector<std::wstring> substrings;

	size_t pos = 0;

	while (pos < inputStr.length()) {
		size_t next_delim = inputStr.find_first_of(delimiter, pos);
		if (next_delim == std::wstring::npos) {
			next_delim = inputStr.length();
		}
		if (inputStr.substr(pos, next_delim - pos) != L"")
		{
			substrings.push_back(inputStr.substr(pos, next_delim - pos));
		}
		pos = next_delim + 1;
	}

	// Print the resulting substrings vector
	for (const auto& substring : substrings) {
		std::wcout << substring << std::endl;
	}

	return substrings;
}

std::wstring UI_Loader::GetSplitName(std::wstring fullpath)
{
	TCHAR dirve[MAX_PATH] = { 0, };
	TCHAR dir[MAX_PATH] = { 0, };
	TCHAR filename[MAX_PATH] = { 0, };
	TCHAR ext[MAX_PATH] = { 0, };
	_wsplitpath_s(fullpath.c_str(), dirve, dir, filename, ext);

	std::wstring szName(filename);
	std::wstring szExt(ext);
	szName = szName + szExt;

	return szName;
}

void UI_Loader::FileLoad(WidgetComponent* wc, std::wstring fileName)
{
	m_szFileContent = FileReadToString(fileName);
	m_splitContent = SplitString(m_szFileContent, L"="); // 일단 오브젝트 단위로 쪼갬

	UIType			Type;
	int				ID;
	std::wstring	Name;
	Vector2			OrginPos;
	float			Depth;
	float			Alpha;

	std::vector<CI> cutInfoList;
	int				cutNum;
	Vector2			WidthHeight;
	Vector4			uvTL;
	std::wstring	TextureName;

	for (auto obj : m_splitContent)
	{
		std::vector<std::wstring> objContent = SplitString(obj, L"\n");
		if (objContent.size() == 0) { continue; }

		for (auto line : objContent)
		{
			std::vector<std::wstring> LineContent = SplitString(line, L"\t");

			if (LineContent.size() == 0) { continue; }
			else if (LineContent[0] == L"Type") { Type = (UIType)std::stoi(LineContent[1]); }
			else if (LineContent[0] == L"ID") { ID = std::stoi(LineContent[1]); }
			else if (LineContent[0] == L"Name") { Name = LineContent[1]; }
			else if (LineContent[0] == L"OrginPos")
			{
				std::vector<std::wstring> OrginPosContent = SplitString(LineContent[1], L" ");
				OrginPos.x = std::stof(OrginPosContent[0]);
				OrginPos.y = std::stof(OrginPosContent[1]);
			}
			else if (LineContent[0] == L"Depth") { Depth = std::stof(LineContent[1]); }
			else if (LineContent[0] == L"Alpha") { Alpha = std::stof(LineContent[1]); }
			// else if (LineContent[0] == L"CutNum") { cutNum = std::stoi(LineContent[1]); } // 얘는 사실 필요없을지도?
			else if (LineContent[0] == L"CutInfo")
			{
				std::vector<std::wstring> ciContent = SplitString(LineContent[1], L"|");

				CI newCutInfo;

				std::vector<std::wstring> cutNumContent = SplitString(ciContent[0], L" ");
				newCutInfo.cn = std::stoi(cutNumContent[0]);

				std::vector<std::wstring> cutWHContent = SplitString(ciContent[1], L" ");
				newCutInfo.pxWH.x = std::stof(cutWHContent[0]);
				newCutInfo.pxWH.y = std::stof(cutWHContent[1]);

				std::vector<std::wstring> cutUVContent = SplitString(ciContent[2], L" ");
				newCutInfo.uv.x = std::stof(cutUVContent[0]);
				newCutInfo.uv.y = std::stof(cutUVContent[1]);
				newCutInfo.uv.z = std::stof(cutUVContent[2]);
				newCutInfo.uv.w = std::stof(cutUVContent[3]);

				// 여기서 절대경로를 상대경로로 바꿔치기하는 야매술법
				std::vector<std::wstring> cutTNContent = SplitString(ciContent[3], L" ");
				newCutInfo.tn = L"../resource/UI/";
				if (Type == IMAGE) { newCutInfo.tn += L"Image/"; }
				else if (Type == BUTTON) { newCutInfo.tn += L"Button/"; }
				newCutInfo.tn += GetSplitName(cutTNContent[0]);

				cutInfoList.push_back(newCutInfo);
			}
		}

	// 여기서 UI 오브젝트 생성
	WidgetObject* wo = new WidgetObject;
	wo->m_Type = Type;
	wo->SetPosition(OrginPos);
	for (auto CI : cutInfoList) { wo->AddCut(CI.pxWH, CI.uv, CI.tn); }

	wc->AddObj(Name, wo);

	cutInfoList.clear();
	}
}