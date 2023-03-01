#pragma once
#include "LandscapeParentDlg.h"

// LandscapeDockPane

class LandscapeDockPane : public CDockablePane
{
	DECLARE_DYNAMIC(LandscapeDockPane)

public:
	LandscapeDockPane();
	virtual ~LandscapeDockPane();

public:
	LandscapeParentDlg m_ParentDlg;


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


