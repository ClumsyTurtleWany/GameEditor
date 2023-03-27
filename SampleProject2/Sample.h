#pragma once
#include "EditorCore.h"
#include "World.hpp"
#include "Camera.h"

#include "LoadingScene.h"
#include "TitleScene.h"
#include "SelectScene.h"
#include "MapScene.h"
#include "BattleScene.h"
#include "CardViewScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"

ECS_DEFINE_TYPE(SomeComponent);

class SampleCore : public EditorCore
{
public:
	ECS::World MainWorld;
	Camera* DebugCamera;

public:
	SampleCore();
	~SampleCore();

public:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;

public:
	void LoadTexture();
	void LoadBGM();
	void SceneChange();

public:
	BaseScene* CurrentScene;

	LoadingScene*	Loading;
	TitleScene*		Title;
	SelectScene*	Select;
	MapScene*		Map;
	BattleScene*	Battle;
	CardViewScene*	CardView;
	ClearScene*		Clear;
	GameOverScene*	GameOver;

public:
	FMODSound* bgm_Current;
	FMODSound* bgm_Title;
	FMODSound* bgm_Map;
	FMODSound* bgm_Battle;
	FMODSound* bgm_Clear;
	FMODSound* bgm_GameOver;

public:
	Deck* Dick;
	std::vector<DXTexture*> CardTextureList;
	std::vector<DXTexture*> NumberTextureList_Red;
	std::vector<DXTexture*> NumberTextureList_Black;
};