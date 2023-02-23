// OutlinerDockPane.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "OutlinerDockPane.h"


// OutlinerDockPane

IMPLEMENT_DYNAMIC(OutlinerDockPane, CDockablePane)

OutlinerDockPane::OutlinerDockPane()
{

}

OutlinerDockPane::~OutlinerDockPane()
{
}


BEGIN_MESSAGE_MAP(OutlinerDockPane, CDockablePane)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// OutlinerDockPane 메시지 처리기




void OutlinerDockPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ParentDlg.MoveWindow(0, 0, cx, cy);
}


int OutlinerDockPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	m_ParentDlg.Create(IDD_OutlinerParentDlg, this);
	m_ParentDlg.ShowWindow(SW_SHOW);

	return 0;
}


void OutlinerDockPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ParentDlg.SetFocus();
}


void OutlinerDockPane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
}

void OutlinerDockPane::Update()
{
	m_ParentDlg.Update();
}
