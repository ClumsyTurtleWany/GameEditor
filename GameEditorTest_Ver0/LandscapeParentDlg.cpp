// LandscapeParentDlg.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "LandscapeParentDlg.h"
#include "afxdialogex.h"


// LandscapeParentDlg 대화 상자

IMPLEMENT_DYNAMIC(LandscapeParentDlg, CDialog)

LandscapeParentDlg::LandscapeParentDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LandscapeParentDlg, pParent)
{

}

LandscapeParentDlg::~LandscapeParentDlg()
{
}

void LandscapeParentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LandscapeParentDlg, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDB_LandscapeManagement, &OnBtnClickedManagement)
	ON_BN_CLICKED(IDB_LandscapeSculpting, &OnBtnClickedSculpting)
END_MESSAGE_MAP()


// LandscapeParentDlg 메시지 처리기


int LandscapeParentDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void LandscapeParentDlg::OnBtnClickedManagement()
{
	m_ManagementView->ShowWindow(SW_SHOW);
}

void LandscapeParentDlg::OnBtnClickedSculpting()
{
	m_SculptingView->ShowWindow(SW_SHOW);
}


BOOL LandscapeParentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// 버튼 생성
	m_btnManagement.Create(L"Management", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(10, 10, 110, 40), this, IDB_LandscapeManagement);
	m_btnSculpting.Create(L"Sculpting", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(110, 10, 210, 40), this, IDB_LandscapeSculpting);

	// Management View 생성
	CRect rc;
	GetClientRect(&rc);
	rc.top += 50;
	CCreateContext cc;
	CView* pView = (CView*)RUNTIME_CLASS(LandscapeManagementFormView)->CreateObject();
	::ZeroMemory(&cc, sizeof(cc));
	pView->Create(NULL, NULL, WS_CHILD, rc, this, IDD_LandscapeManagementFormView, &cc);
	pView->OnInitialUpdate();
	m_ManagementView = pView;

	// Sculpting View 생성
	pView = (CView*)RUNTIME_CLASS(LandscapeSculptingFormView)->CreateObject();
	::ZeroMemory(&cc, sizeof(cc));
	pView->Create(NULL, NULL, WS_CHILD, rc, this, IDD_LandscapeSculptingFormView, &cc);
	pView->OnInitialUpdate();
	m_SculptingView = pView;


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
