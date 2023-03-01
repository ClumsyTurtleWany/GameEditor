// OutlinerParentDlg.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "OutlinerParentDlg.h"
#include "afxdialogex.h"
#include "Entity.hpp"


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
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_OUTLINER_TREE, &OutlinerParentDlg::OnTvnSelchangedOutlinerTree)
	ON_WM_KEYDOWN()
	ON_NOTIFY(TVN_KEYDOWN, IDC_OUTLINER_TREE, &OutlinerParentDlg::OnTvnKeydownOutlinerTree)
END_MESSAGE_MAP()


// OutlinerParentDlg 메시지 처리기


BOOL OutlinerParentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	Root = OutlinerTree.InsertItem(L"Root", NULL, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void OutlinerParentDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (OutlinerTree.GetSafeHwnd() != NULL)
	{
		OutlinerTree.MoveWindow(5, 5, cx - 10, cy - 10);
	}

}

void OutlinerParentDlg::OnTvnSelchangedOutlinerTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strName;
	HTREEITEM selectedItem = OutlinerTree.GetSelectedItem();
	strName = OutlinerTree.GetItemText(selectedItem);
	//int id = _ttoi(strName);

	if (theApp.m_TestClass != nullptr)
	{
		for (auto& it : theApp.m_TestClass->World.GetAllEntities())
		{
			ECS::Entity* entity = it.get();
			CString entityName = CString::CStringT(entity->GetName().c_str());
			if (strName == entityName)
			{
				theApp.SelectEntity(entity);
				break;
			}
		}
	}


	*pResult = 0;
}

void OutlinerParentDlg::Update()
{
	OutlinerTree.DeleteAllItems();
	Root = OutlinerTree.InsertItem(L"Root", NULL, NULL);
	for (auto& it : theApp.m_TestClass->World.GetAllEntities())
	{
		ECS::Entity* entity = it.get();
		//CString str;
		//str.Format(_T("%d"), entity->GetID());
		CString strName = CString::CStringT(entity->GetName().c_str());
		OutlinerTree.InsertItem(strName, NULL, NULL, Root);
	}

	RedrawWindow();
}


void OutlinerParentDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nChar)
	{
	case VK_DELETE:
	{
		if (theApp.SelectedEntity != nullptr)
		{
			theApp.SelectedEntity->IsDestroy = true;
		}
	}
	break;
	}
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

void OutlinerParentDlg::OnTvnKeydownOutlinerTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (pTVKeyDown->wVKey == VK_DELETE)
	{
		CString strName;
		HTREEITEM selectedItem = OutlinerTree.GetSelectedItem();
		strName = OutlinerTree.GetItemText(selectedItem);
		OutlinerTree.DeleteItem(selectedItem);
	
		if (theApp.m_TestClass != nullptr)
		{
			for (auto& it : theApp.m_TestClass->World.GetAllEntities())
			{
				ECS::Entity* entity = it.get();
				CString entityName = CString::CStringT(entity->GetName().c_str());
				if (strName == entityName)
				{
					entity->IsDestroy = true;
					theApp.SelectEntity(nullptr);
					break;
				}
			}
		}
		RedrawWindow();
	}

	*pResult = 0;
}
