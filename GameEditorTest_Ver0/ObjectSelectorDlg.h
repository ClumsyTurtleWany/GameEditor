#pragma once
#include "afxdialogex.h"


// ObjectSelectorDlg 대화 상자

class ObjectSelectorDlg : public CDialogEx
{
public:
	

	DECLARE_DYNAMIC(ObjectSelectorDlg)

public:
	ObjectSelectorDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~ObjectSelectorDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ObjectSelectorDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	CButton BtnDirectionalLight;
	CButton BtnSpotLight;
	CButton BtnPointLight;

	CListBox ObjectListBox;
	CButton BtnSelectObject;
	CButton BtnAddObject;

	bool isInitialize = false;

	afx_msg void OnBnClickedButtonDirectionalLight();
	afx_msg void OnBnClickedButtonSpotLight();
	afx_msg void OnBnClickedButtonPointLight();
	afx_msg void OnBnClickedButtonSelectObject();
	afx_msg void OnBnClickedButtonAddObject();
	afx_msg void OnLbnSelchangeListObjectSelector();

	void Initialize();
};
