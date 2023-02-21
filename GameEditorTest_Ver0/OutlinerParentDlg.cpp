// OutlinerParentDlg.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "OutlinerParentDlg.h"
#include "afxdialogex.h"


// OutlinerParentDlg 대화 상자

IMPLEMENT_DYNAMIC(OutlinerParentDlg, CDialogEx)

OutlinerParentDlg::OutlinerParentDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OutlinerParentDlg, pParent)
{

}

OutlinerParentDlg::~OutlinerParentDlg()
{
}

void OutlinerParentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTLINER_TREE, OutlinerTree);
}


BEGIN_MESSAGE_MAP(OutlinerParentDlg, CDialogEx)
END_MESSAGE_MAP()


// OutlinerParentDlg 메시지 처리기


BOOL OutlinerParentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	HTREEITEM root;
	root = OutlinerTree.InsertItem(L"Root", NULL, NULL);
	theApp.m_TestClass->World.GetAllEntities();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
