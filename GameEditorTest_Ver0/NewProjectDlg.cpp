// NewProjectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "NewProjectDlg.h"
#include "afxdialogex.h"


// NewProjectDlg 대화 상자

IMPLEMENT_DYNAMIC(NewProjectDlg, CDialogEx)

NewProjectDlg::NewProjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NewProjectDlg, pParent)
{

}

NewProjectDlg::~NewProjectDlg()
{
}

void NewProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NewProjectDlg, CDialogEx)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// NewProjectDlg 메시지 처리기


BOOL NewProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.


	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL NewProjectDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	BOOL bStatus = FALSE;
	if (pDC)
	{
		CRect clRect;
		GetClientRect(&clRect);
		CBrush clBrush(RGB(36, 36, 36));
		CBrush* pOldBrush = pDC->SelectObject(&clBrush);
		pDC->FillRect(&clRect, &clBrush);
		pDC->SelectObject(pOldBrush);
		bStatus = TRUE;
	}
	return bStatus;

	//return CDialogEx::OnEraseBkgnd(pDC);
}
