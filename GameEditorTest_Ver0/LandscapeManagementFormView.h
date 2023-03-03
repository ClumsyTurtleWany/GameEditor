#pragma once
#include "FBXLoader.hpp"



// LandscapeManagementFormView 폼 보기

class LandscapeManagementFormView : public CFormView
{
	DECLARE_DYNCREATE(LandscapeManagementFormView)

protected:
	LandscapeManagementFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LandscapeManagementFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LandscapeManagementFormView };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

public:
	int SectionSizeMode = 0;
	int ComponentHeight = 8;
	int ComponentWidth = 8;

	CEdit EditComponentWidth;
	CEdit EditComponentHeight;
	CButton BtnCreateLandscape;
	CComboBox SectionSizeComboBox;

public:
	virtual void OnDraw(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnEnChangeLandscapeComponentWidth();
	afx_msg void OnEnChangeLandscapeComponentHeight();
	afx_msg void OnBnClickedBtnCreateLandscape();

	void Initialize();
	afx_msg void OnCbnSelchangeComboLandscapeSectionSize();
};


