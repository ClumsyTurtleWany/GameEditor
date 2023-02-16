// LandscapeDockPane.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "LandscapeDockPane.h"


// LandscapeDockPane

IMPLEMENT_DYNAMIC(LandscapeDockPane, CDockablePane)

LandscapeDockPane::LandscapeDockPane()
{

}

LandscapeDockPane::~LandscapeDockPane()
{
}


BEGIN_MESSAGE_MAP(LandscapeDockPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// LandscapeDockPane 메시지 처리기




int LandscapeDockPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ParentDlg.Create(IDD_LandscapeParentDlg, this);
	m_ParentDlg.ShowWindow(SW_SHOW);

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void LandscapeDockPane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.

	CRect rectDlg;
	m_ParentDlg.GetWindowRect(rectDlg);
	ScreenToClient(rectDlg);

}


void LandscapeDockPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ParentDlg.MoveWindow(0, 0, cx, cy);
}


void LandscapeDockPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_ParentDlg.SetFocus();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
