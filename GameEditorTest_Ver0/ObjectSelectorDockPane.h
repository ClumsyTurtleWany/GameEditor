#pragma once
#include "ObjectSelectorDlg.h"

// ObjectSelectorDockPane

class ObjectSelectorDockPane : public CDockablePane
{
public:
	ObjectSelectorDlg m_ParentDlg;

	DECLARE_DYNAMIC(ObjectSelectorDockPane)

public:
	ObjectSelectorDockPane();
	virtual ~ObjectSelectorDockPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


