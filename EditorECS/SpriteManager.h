#pragma once
#include "EffectDefine.h"

namespace EFFECTUTIL
{
	struct UVRECT
	{
		union
		{
			struct
			{
				float l, b, r, t;
			};

			struct
			{
				Vector2 vMin;
				Vector2 vMax;
			};
		};
		UVRECT()
		{
			vMax = vMin = { 0.0f, 0.0f };
		}

		UVRECT(float l, float b, float r, float t)
		{
			this->l = l;
			this->b = b;
			this->r = r;
			this->t = t;
		}

		UVRECT(Vector2 vMin, Vector2 vMax)
		{
			this->vMin = vMin;
			this->vMax = vMax;
		}
	};

	using UV_ARRAY = std::vector<UVRECT>;
	using TEX_ARRAY = std::vector<DXTexture*>;

	enum SPRITE_TYPE
	{
		SPRITE_UV = 0,
		SPRITE_MULTI_TEX,
		NUMBER_OF_SPRITES
	};

	struct SPRITE_INFO
	{
		SPRITE_TYPE		m_spriteType;

		//UV
		UINT					m_iRows;
		UINT					m_iCols;
		float					m_fSingleRowLen;
		float					m_fSingleColLen;
		UVRECT					m_texRECT;

		//COMMON
		UINT					m_iStFrame;
		UINT					m_iEdFrame;
		UINT					m_iTotalFrame;
	};

	struct EffectSprite
	{
		std::wstring			m_wszName;
		std::wstring			m_wszUVTexName;
		std::wstring			m_wszTexFilePath;
		SPRITE_INFO				m_spriteInfo;

		UV_ARRAY				m_UVSpriteArr;
		TEX_ARRAY				m_pTexArr;
	};

	class SpriteManager
	{
	private:
		std::set<std::wstring>					m_scriptFilenameSet;
		std::map<std::wstring, EffectSprite*>	m_UVSpriteMap;
		std::map<std::wstring, EffectSprite*>	m_MTspriteMap;

		SpriteManager();
		~SpriteManager();

	public:
		std::wstring							m_wszDefaultUVScriptFilePath;
		std::wstring							m_wszDefaultMTScriptFilePath;

		SpriteManager(const SpriteManager& other) = delete;
		SpriteManager& operator=(const SpriteManager& other) = delete;

	public:
		bool init();
		bool release();

		bool createUVSprite(std::wstring wszSpriteName,
			std::wstring wszTexName,
			std::wstring wszTexFilepath,
			UINT iRows, UINT iCols);

		bool createMTSprite(std::wstring wszSpriteName,
			std::vector<std::wstring>* pWszTexNames,
			std::vector<std::wstring>* pWszTexFilepaths);

		int loadUVScript(std::wstring wszUVScriptFilepath);
		int saveUVScript(std::wstring wszUVScriptFilepath);

		int loadMTScript(std::wstring wszMTScriptFilepath);
		int saveMTScript(std::wstring wszMTScriptFilepath);

		EffectSprite* getUVPtr(std::wstring wszSpriteName);
		EffectSprite* getMTPtr(std::wstring wszSpriteName);

		void getUVSpriteNames(std::vector<std::wstring>& wszNameList);
		void getUVSpriteNames(std::vector<std::string>& szNameList);

		void getMTSpriteNames(std::vector<std::wstring>& wszNameList);
		void getMTSpriteNames(std::vector<std::string>& szNameList);

		static SpriteManager& getInstance()
		{
			static SpriteManager singleInst;
			return singleInst;
		}
	};

	#define SPRITE_MGR SpriteManager::getInstance()

}