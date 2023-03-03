// LandscapeSculptingFormView.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "LandscapeSculptingFormView.h"


// LandscapeSculptingFormView

IMPLEMENT_DYNCREATE(LandscapeSculptingFormView, CFormView)

LandscapeSculptingFormView::LandscapeSculptingFormView()
	: CFormView(IDD_LandscapeSculptingFormView)
{
	
}

LandscapeSculptingFormView::~LandscapeSculptingFormView()
{
}

void LandscapeSculptingFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LandscapeSculptingFormView, CFormView)
END_MESSAGE_MAP()


// LandscapeSculptingFormView 진단

#ifdef _DEBUG
void LandscapeSculptingFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void LandscapeSculptingFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// LandscapeSculptingFormView 메시지 처리기
