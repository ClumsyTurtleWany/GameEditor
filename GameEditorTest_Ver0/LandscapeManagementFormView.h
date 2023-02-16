#pragma once



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
};


