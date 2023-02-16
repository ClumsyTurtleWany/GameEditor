#pragma once
#include "LandscapeManagementFormView.h"
#include "LandscapeSculptingFormView.h"

// LandscapeParentDlg 대화 상자

class LandscapeParentDlg : public CDialog
{
	DECLARE_DYNAMIC(LandscapeParentDlg)

public:
	LandscapeParentDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~LandscapeParentDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LandscapeParentDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnClickedManagement();
	afx_msg void OnBtnClickedSculpting();

public:
	CButton m_btnManagement;
	CView* m_ManagementView;

	CButton m_btnSculpting;
	CView* m_SculptingView;

public:
	virtual BOOL OnInitDialog();
};
