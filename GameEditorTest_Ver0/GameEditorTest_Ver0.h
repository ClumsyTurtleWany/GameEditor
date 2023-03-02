// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// GameEditorTest_Ver0.h: GameEditorTest_Ver0 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "TestClass.h"

// CGameEditorTestVer0App:
// 이 클래스의 구현에 대해서는 GameEditorTest_Ver0.cpp을(를) 참조하세요.
//
enum class EditorMode
{
	Selector,
	Sculpting,
	Splatting,
};

class CGameEditorTestVer0App : public CWinAppEx
{
public:
	CGameEditorTestVer0App() noexcept;
	TestClass*	m_TestClass;
	ECS::Entity* SelectedEntity = nullptr;
	EditorMode  CurrentEditorMode = EditorMode::Selector;

public:
	std::wstring ProjectName;
	std::wstring ProjectPath;

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);

public:
	void Update();
	void SelectEntity(ECS::Entity* entity);

public:
	//-------------------------------------
	// Project
	//-------------------------------------
	bool NewProject();
	bool SaveProject();
	bool SaveAsProject();
	bool LoadProject();

	//-------------------------------------
	// Level
	//-------------------------------------
	bool NewLevel();
	bool SaveLevel();
	bool SaveAsLevel();
	bool LoadLevel();

};

extern CGameEditorTestVer0App theApp;
