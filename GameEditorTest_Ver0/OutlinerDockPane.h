#pragma once
#include "OutlinerParentDlg.h"

// OutlinerDockPane

class OutlinerDockPane : public CDockablePane
{
	DECLARE_DYNAMIC(OutlinerDockPane)

public:
	OutlinerParentDlg m_ParentDlg;
	
public:
	OutlinerDockPane();
	virtual ~OutlinerDockPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();

public:
	void Update();
};


