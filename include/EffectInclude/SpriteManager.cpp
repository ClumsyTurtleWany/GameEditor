#include "SpriteManager.h"

namespace EFFECTUTIL
{
	SpriteManager::SpriteManager()
	{
	}

	SpriteManager::~SpriteManager()
	{
	}

	bool SpriteManager::init()
	{
		m_wszDefaultUVScriptFilePath = L"../resource/EffectPart/save/script/spritescript/UVSpriteScript.UVSTXT";
		m_wszDefaultMTScriptFilePath = L"../resource/EffectPart/save/script/spritescript/MTSpriteScript.MTSTXT";

		loadUVScript(m_wszDefaultUVScriptFilePath);
		loadMTScript(m_wszDefaultMTScriptFilePath);

		return true;
	}

	bool SpriteManager::release()
	{
		m_scriptFilenameSet.clear();

		for (auto it : m_UVSpriteMap)
		{
			delete it.second;
			it.second = nullptr;
		}

		for (auto it : m_MTspriteMap)
		{
			delete it.second;
			it.second = nullptr;
		}

		m_UVSpriteMap.clear();
		m_MTspriteMap.clear();

		return true;
	}

	bool SpriteManager::createUVSprite(std::wstring wszSpriteName, std::wstring wszTexName, std::wstring wszTexFilepath, UINT iRows, UINT iCols)
	{
		if (m_UVSpriteMap.find(wszSpriteName) == m_UVSpriteMap.end())
		{
			bool bRet = DXTextureManager::GetInstance()->Load(wszTexFilepath);
			if (!bRet) { return false; }

			EffectSprite* newE = new EffectSprite;
			newE->m_wszName = wszSpriteName;
			newE->m_wszUVTexName = wszTexName;
			newE->m_wszTexFilePath = wszTexFilepath;
			newE->m_pTexArr.push_back(DXTextureManager::GetInstance()->GetTexture(wszTexFilepath));

			D3D11_TEXTURE2D_DESC td;
			newE->m_pTexArr[0]->GetTexture2D()->GetDesc(&td);
			float singleCol = td.Width / (float)iCols;
			float singleRow = td.Height / (float)iRows;


			newE->m_spriteInfo.m_spriteType = SPRITE_UV;
			newE->m_spriteInfo.m_iRows = iRows;
			newE->m_spriteInfo.m_iCols = iCols;
			newE->m_spriteInfo.m_fSingleRowLen = singleCol;
			newE->m_spriteInfo.m_fSingleColLen = singleRow;
			newE->m_spriteInfo.m_texRECT = { {0.0f, 0.0f}, {(float)td.Width, (float)td.Height} };

			newE->m_spriteInfo.m_iStFrame = 0;
			newE->m_spriteInfo.m_iEdFrame = iRows * iCols - 1;
			newE->m_spriteInfo.m_iTotalFrame = iRows * iCols;

			for (UINT i = 0; i < iRows; i++)
			{
				for (UINT j = 0; j < iCols; j++)
				{
					newE->m_UVSpriteArr.push_back({ {(singleCol / td.Width) * j, (singleRow / td.Height) * i},
						{(singleCol / td.Width) * (j + 1), (singleRow / td.Height) * (i + 1)} });
				}
			}

			m_UVSpriteMap.insert(std::make_pair(wszSpriteName, newE));
		}
		else { return false; }

		return true;
	}

	bool SpriteManager::createMTSprite(std::wstring wszSpriteName,
		std::vector<std::wstring>* pWszTexNames,
		std::vector<std::wstring>* pWszTexFilepaths)
	{
		if (m_MTspriteMap.find(wszSpriteName) == m_MTspriteMap.end())
		{
			if (pWszTexNames && pWszTexFilepaths)
			{
				EffectSprite* newE = new EffectSprite;

				for (int i = 0; i < pWszTexFilepaths->size(); i++)
				{
					if (FAILED(DXTextureManager::GetInstance()->Load((*pWszTexFilepaths)[i])))
					{
						if (!DXTextureManager::GetInstance()->GetTexture((*pWszTexFilepaths)[i]))
						{
							newE->m_pTexArr.clear();
							delete newE;
							return false;
						}
					}

					newE->m_pTexArr.push_back(DXTextureManager::GetInstance()->GetTexture((*pWszTexFilepaths)[i]));
				}

				newE->m_wszName = wszSpriteName;

				newE->m_spriteInfo.m_spriteType = SPRITE_MULTI_TEX;

				newE->m_spriteInfo.m_iStFrame = 0;
				newE->m_spriteInfo.m_iEdFrame = newE->m_pTexArr.size() - 1;
				newE->m_spriteInfo.m_iTotalFrame = newE->m_pTexArr.size();

				m_MTspriteMap.insert(std::make_pair(wszSpriteName, newE));
			}
			else { return false; }
		}
		else { return false; }
		return true;
	}

	EffectSprite* SpriteManager::getUVPtr(std::wstring wszSpriteName)
	{
		auto it = m_UVSpriteMap.find(wszSpriteName);
		if (it != m_UVSpriteMap.end()) { return it->second; }

		return nullptr;
	}

	EffectSprite* SpriteManager::getMTPtr(std::wstring wszSpriteName)
	{
		auto it = m_MTspriteMap.find(wszSpriteName);
		if (it != m_MTspriteMap.end()) { return it->second; }

		return nullptr;
	}

	void SpriteManager::getUVSpriteNames(std::vector<std::wstring>& wszNameList)
	{
		wszNameList.clear();

		for (auto it : m_UVSpriteMap)
		{
			wszNameList.push_back(it.first);
		}
	}

	void SpriteManager::getMTSpriteNames(std::vector<std::wstring>& wszNameList)
	{
		wszNameList.clear();

		for (auto it : m_MTspriteMap)
		{
			wszNameList.push_back(it.first);
		}
	}

	void SpriteManager::getUVSpriteNames(std::vector<std::string>& szNameList)
	{
		szNameList.clear();

		for (auto it : m_UVSpriteMap)
		{
			szNameList.push_back(atl_W2M(it.first));
		}
	}

	void SpriteManager::getMTSpriteNames(std::vector<std::string>& szNameList)
	{
		szNameList.clear();

		for (auto it : m_MTspriteMap)
		{
			szNameList.push_back(atl_W2M(it.first));
		}
	}

	int SpriteManager::loadUVScript(std::wstring wszUVScriptFilepath)
	{
		//스크립트 파일명 중복 확인
		if (m_scriptFilenameSet.end() != m_scriptFilenameSet.find(wszUVScriptFilepath)) { return false; }

		//필요 변수 선언
		std::wifstream wiFp;
		wchar_t	scriptInfo[256] = { 0 };

		std::wstring wszCheckHeadStr;

		std::wstring wszSpriteName;
		std::wstring wszTexName;
		std::wstring wszTexFilepath;
		UINT iRows;
		UINT iCols;

		//카운트와 ID인덱스
		UINT cnt = 0;		//불러오기를 성공한 리소스 개수 반환용

		//파일 열기
		wiFp.open(wszUVScriptFilepath, std::ios::in);
		if (!wiFp) { return cnt; }

		wiFp >> wszCheckHeadStr;

		if (wszCheckHeadStr != L"#UVScript")
		{
			wiFp.close();
			return cnt;
		}

		//리소스 읽기 루프
		while (true)
		{
			bool bRet = true;

			//이름인지 끝인지 확인
			wiFp >> wszSpriteName;
			if (wszSpriteName == L"#END") { break; }
			else
			{
				wiFp >> wszTexName;
				wiFp >> wszTexFilepath;
				wiFp >> iRows;
				wiFp >> iCols;

				bRet = createUVSprite(wszSpriteName,
					wszTexFilepath,
					wszTexFilepath,
					iRows,
					iCols);

				DXTextureManager::GetInstance()->Load(wszTexFilepath);

				if (bRet) { cnt++; }
			}
		}

		wiFp.close();

		//루프가 끝나면 파일 매니저에 등록
		m_scriptFilenameSet.insert(wszUVScriptFilepath);

		return cnt;
	}

	int SpriteManager::saveUVScript(std::wstring wszUVScriptFilepath)
	{
		//필요 변수 선언
		std::wofstream woFp;
		wchar_t	scriptInfo[256] = { 0 };

		//카운트
		UINT cnt = 0;		//불러오기를 성공한 리소스 개수 반환용

		//파일 열기
		woFp.open(wszUVScriptFilepath, std::ios::out);
		if (!woFp) { return cnt; }

		woFp << L"#UVScript";
		woFp << L"\n";

		//리소스 쓰기 루프
		for (auto it : m_UVSpriteMap)
		{
			woFp << it.second->m_wszName;
			woFp << L"\n";

			woFp << it.second->m_wszUVTexName;
			woFp << L"\n";

			woFp << it.second->m_wszTexFilePath;
			woFp << L"\n";

			woFp << it.second->m_spriteInfo.m_iRows;
			woFp << L"\n";

			woFp << it.second->m_spriteInfo.m_iCols;
			woFp << L"\n";

			cnt++;
		}

		woFp << L"#END";

		woFp.close();

		return cnt;
	}

	int SpriteManager::loadMTScript(std::wstring wszMTScriptFilepath)
	{
		//스크립트 파일명 중복 확인
		if (m_scriptFilenameSet.end() != m_scriptFilenameSet.find(wszMTScriptFilepath)) { return false; }

		//필요 변수 선언
		std::wifstream wiFp;
		wchar_t	scriptInfo[256] = { 0 };

		std::wstring wszCheckHeadStr;

		std::wstring wszSpriteName;
		std::wstring wszTexName;
		std::wstring wszTexFilepath;
		std::vector<std::wstring> wszTexNameList;
		std::vector<std::wstring> wszTexFilepathList;

		//카운트와 ID인덱스
		UINT cnt = 0;		//불러오기를 성공한 리소스 개수 반환용

		//파일 열기
		wiFp.open(wszMTScriptFilepath, std::ios::in);
		if (!wiFp) { return cnt; }

		wiFp >> wszCheckHeadStr;

		if (wszCheckHeadStr != L"#MTScript")
		{
			wiFp.close();
			return cnt;
		}

		//리소스 읽기 루프
		while (true)
		{
			bool bRet = true;

			//이름인지 끝인지 확인
			wiFp >> wszSpriteName;
			if (wszSpriteName == L"#END") { break; }
			else
			{
				wiFp >> wszTexName;
				while (wszTexName != L"#TEXLISTEND")
				{
					wszTexNameList.push_back(wszTexName);
					wiFp >> wszTexName;
				}

				int N = 0;
				wiFp >> wszTexFilepath;
				while (wszTexFilepath != L"#TEXPATHLISTEND")
				{
					wszTexFilepathList.push_back(wszTexFilepath);
					DXTextureManager::GetInstance()->Load(wszTexFilepath);

					wiFp >> wszTexFilepath;
				}

				bRet = createMTSprite(wszSpriteName,
					&wszTexFilepathList,
					&wszTexFilepathList);

				if (bRet) { cnt++; }

				wszTexNameList.clear();
				wszTexFilepathList.clear();
			}
		}

		wiFp.close();

		//루프가 끝나면 파일 매니저에 등록
		m_scriptFilenameSet.insert(wszMTScriptFilepath);

		return cnt;
	}

	int SpriteManager::saveMTScript(std::wstring wszMTScriptFilepath)
	{
		//필요 변수 선언
		std::wofstream woFp;
		wchar_t	scriptInfo[256] = { 0 };

		//카운트
		UINT cnt = 0;		//불러오기를 성공한 리소스 개수 반환용

		//파일 열기
		woFp.open(wszMTScriptFilepath, std::ios::out);
		if (!woFp) { return cnt; }

		woFp << L"#MTScript";
		woFp << L"\n";

		//리소스 쓰기 루프
		for (auto it : m_MTspriteMap)
		{
			woFp << it.second->m_wszName;
			woFp << L"\n";

			for (auto Texit : it.second->m_pTexArr)
			{
				woFp << Texit->GetFileName();
				woFp << L"\n";
			}

			woFp << L"#TEXLISTEND\n";

			for (auto Texit : it.second->m_pTexArr)
			{
				woFp << Texit->GetFileName();
				woFp << L"\n";
			}

			woFp << L"#TEXPATHLISTEND\n";
		}

		woFp << L"#END";

		woFp.close();

		return cnt;
	}
}