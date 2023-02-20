#pragma once


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
	virtual BOOL OnInitDialog();
};
