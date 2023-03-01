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
	afx_msg void OnBtnClickedFbxSelect();
	afx_msg void OnComboChangedSection();
	afx_msg void OnEditChangedCompWidth();
	afx_msg void OnEditChangedCompHeight();
	afx_msg void OnBtnClickedBuildLandscape();
	afx_msg void OnSelChangeFbxFile();

public:
	float LocationX;
	float LocationY;
	float LocationZ;
	float RotationX;
	float RotationY;
	float RotationZ;
	float ScaleX;
	float ScaleY;
	float ScaleZ;

public:
	CStatic* StaticLocation;
	CStatic* StaticRotation;
	CStatic* StaticScale;

	CEdit* EditLocationX;
	CEdit* EditLocationY;
	CEdit* EditLocationZ;

	CEdit* EditRotationX;
	CEdit* EditRotationY;
	CEdit* EditRotationZ;

	CEdit* EditScaleX;
	CEdit* EditScaleY;
	CEdit* EditScaleZ;


	CStatic* StaticSection;
	CComboBox* ComboSection;

	CStatic* StaticComponent;
	CEdit* EditCompWidth;
	CEdit* EditCompHeight;

	CButton* BtnBuildLandscape;

	//CListBox* FbxFileListBox;
	CButton* BtnFbxSelect;
	
	virtual void OnDraw(CDC* pDC);
	
	CListBox FBXFileListBox;
	afx_msg void OnLbnSelchangeFbxFileList();
};


