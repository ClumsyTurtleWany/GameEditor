#pragma once


// NewProjectDlg 대화 상자

class NewProjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(NewProjectDlg)

public:
	NewProjectDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~NewProjectDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NewProjectDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
