#pragma once
#include "Entity.hpp"

// DetailDockPane

class DetailDockPane : public CDockablePane
{
	DECLARE_DYNAMIC(DetailDockPane)

	CFont m_fntPropList;
	CMFCPropertyGridCtrl m_PropList;
	ECS::Entity* TargetEntity = nullptr;

public:
	DetailDockPane();
	virtual ~DetailDockPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

public:
	void SetFont();
	void UpdateDetail(ECS::Entity* entity);

public:
	LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);
};


