// ObjectSelectorDockPane.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "ObjectSelectorDockPane.h"


// ObjectSelectorDockPane

IMPLEMENT_DYNAMIC(ObjectSelectorDockPane, CDockablePane)

ObjectSelectorDockPane::ObjectSelectorDockPane()
{

}

ObjectSelectorDockPane::~ObjectSelectorDockPane()
{

}


BEGIN_MESSAGE_MAP(ObjectSelectorDockPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// ObjectSelectorDockPane 메시지 처리기




int ObjectSelectorDockPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_ParentDlg.Create(IDD_ObjectSelectorDlg, this);
	m_ParentDlg.Initialize();
	m_ParentDlg.ShowWindow(SW_SHOW);

	return 0;
}


void ObjectSelectorDockPane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CRect rectDlg;
	m_ParentDlg.GetWindowRect(rectDlg);
	ScreenToClient(rectDlg);
}


void ObjectSelectorDockPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ParentDlg.MoveWindow(0, 0, cx, cy);
}


void ObjectSelectorDockPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_ParentDlg.SetFocus();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
