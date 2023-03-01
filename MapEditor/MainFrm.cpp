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

// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MapEditor.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()

	ON_COMMAND(ID_COMBO_EDITOR_MODE, OnComboChangeEditorMode)
	ON_UPDATE_COMMAND_UI(ID_COMBO_EDITOR_MODE, OnUpdateComboChangeEditorMode)

END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	//m_wndRibbonBar.Create(this);
	//m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
	InitializeRibbonBar();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	CreateDockingWindows();

	m_wndLandscapeDockPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndLandscapeDockPane);
	m_wndLandscapeDockPane.ShowPane(FALSE, FALSE, FALSE);

	m_wndOutlinerDockPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutlinerDockPane);

	m_wndDetailDockPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndDetailDockPane);


	// 모든 사용자 인터페이스 요소를 그리는 데 사용하는 비주얼 관리자를 설정합니다.
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// 비주얼 관리자에서 사용하는 비주얼 스타일을 설정합니다.
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



void CMainFrame::OnComboChangeEditorMode()
{
	if (m_pComboEditorMode)
	{
		int idx = m_pComboEditorMode->GetCurSel();
		if (idx == 0)
		{
			m_wndLandscapeDockPane.ShowPane(FALSE, FALSE, FALSE);
		}
		else if (idx == 1)
		{
			m_wndLandscapeDockPane.ShowPane(TRUE, FALSE, TRUE);
		}
	}
}

void CMainFrame::OnUpdateComboChangeEditorMode(CCmdUI* pcmdui)
{
	pcmdui->Enable(TRUE);
}

bool CMainFrame::InitializeRibbonBar()
{
	m_wndRibbonBar.Create(this);

	CMFCRibbonCategory* pCategory = nullptr;
	CMFCRibbonPanel* pPanel = nullptr;

	pCategory = m_wndRibbonBar.AddCategory(L"Test Ribbon Menu", IDB_WRITESMALL, IDB_WRITELARGE);
	pPanel = pCategory->AddPanel(L"Test");

	//pPanel->Add(new CMFCRibbonButton(IDB_FILE_SAVE, L"Save", theApp.LoadIconW(IDI_FILE_SAVE)));

	pPanel->Add(new CMFCRibbonLabel(L" "));
	m_pComboEditorMode = new CMFCRibbonComboBox(ID_COMBO_EDITOR_MODE, FALSE, -1, L"Mode", -1);
	m_pComboEditorMode->AddItem(L"Select Mode");
	m_pComboEditorMode->AddItem(L"Landscape");
	m_pComboEditorMode->SelectItem(0);
	pPanel->Add(m_pComboEditorMode);

	return false;
}

bool CMainFrame::CreateDockingWindows()
{
	// Landscape Management Dock Pane
	if (!m_wndLandscapeDockPane.Create(L"Landscape", this, CRect(0, 0, 200, 200), TRUE, ID_LANDSCAPE_DOCKPANE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Landscape Management 창을 만들지 못했습니다.\n");
		return false; // 만들지 못했습니다.
	}

	// Outliner Dock Pane
	if (!m_wndOutlinerDockPane.Create(L"Outliner", this, CRect(0, 0, 200, 200), TRUE, ID_OutlinerDockPane, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Outliner 창을 만들지 못했습니다.\n");
		return false; // 만들지 못했습니다.
	}

	// Detail Dock Pane
	if (!m_wndDetailDockPane.Create(L"Detail", this, CRect(0, 0, 200, 200), TRUE, ID_DetailDockPane, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Detail 창을 만들지 못했습니다.\n");
		return false; // 만들지 못했습니다.
	}

	return true;
}

void CMainFrame::UpdateOutliner()
{
	m_wndOutlinerDockPane.Update();
}