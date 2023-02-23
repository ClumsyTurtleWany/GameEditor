#pragma once
//#include "OutlinerTreeView.h"

// OutlinerParentDlg 대화 상자

class OutlinerParentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(OutlinerParentDlg)

public:
	OutlinerParentDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~OutlinerParentDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OutlinerParentDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl OutlinerTree;
	HTREEITEM Root;

public:
	//OutlinerTreeView m_TreeView;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedOutlinerTree(NMHDR* pNMHDR, LRESULT* pResult);

public:
	void Update();
};
