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

public:
	CButton BtnManagement;
	CButton BtnSculpting;
	CButton BtnPaint;

	CView* m_ManagementView;
	CView* m_SculptingView;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedLandscapeManagement();
	
	afx_msg void OnBnClickedLandscapeSculpting();
	afx_msg void OnBnClickedLandscapePainting();
};
