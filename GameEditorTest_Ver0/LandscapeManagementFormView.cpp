// LandscapeManagementFormView.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "LandscapeManagementFormView.h"


// LandscapeManagementFormView

IMPLEMENT_DYNCREATE(LandscapeManagementFormView, CFormView)

LandscapeManagementFormView::LandscapeManagementFormView()
	: CFormView(IDD_LandscapeManagementFormView)
{

}

LandscapeManagementFormView::~LandscapeManagementFormView()
{
}

void LandscapeManagementFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LandscapeManagementFormView, CFormView)
END_MESSAGE_MAP()


// LandscapeManagementFormView 진단

#ifdef _DEBUG
void LandscapeManagementFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void LandscapeManagementFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// LandscapeManagementFormView 메시지 처리기


void LandscapeManagementFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//---------------------------
	// Location
	//---------------------------
	StaticLocation = new CStatic();
	StaticLocation->Create(L"Location", WS_CHILD | WS_VISIBLE /*| SS_CENTER*/, CRect(10, 10, 110, 40), this);

	EditLocationX = new CEdit();
	EditLocationX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(110, 10, 160, 40), this, ID_EDIT_LOCATION_X);
	
	EditLocationY = new CEdit();
	EditLocationY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(160, 10, 210, 40), this, ID_EDIT_LOCATION_Y);

	EditLocationZ = new CEdit(); 
	EditLocationZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(210, 10, 260, 40), this, ID_EDIT_LOCATION_Z);

	//---------------------------
	// Rotation
	//---------------------------
	StaticRotation = new CStatic();
	StaticRotation->Create(L"Rotation", WS_CHILD | WS_VISIBLE /*| SS_CENTER*/, CRect(10, 40, 110, 70), this);

	EditRotationX = new CEdit();
	EditRotationX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(110, 40, 160, 70), this, ID_EDIT_ROTATION_X);

	EditRotationY = new CEdit();
	EditRotationY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(160, 40, 210, 70), this, ID_EDIT_ROTATION_Y);

	EditRotationZ = new CEdit();
	EditRotationZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(210, 40, 260, 70), this, ID_EDIT_ROTATION_Z);

	//---------------------------
	// Scale
	//---------------------------
	StaticScale = new CStatic();
	StaticScale->Create(L"Scale", WS_CHILD | WS_VISIBLE /*| SS_CENTER*/, CRect(10, 70, 110, 100), this);

	EditScaleX = new CEdit();
	EditScaleX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(110, 70, 160, 100), this, ID_EDIT_SCALE_X);

	EditScaleY = new CEdit();
	EditScaleY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(160, 70, 210, 100), this, ID_EDIT_SCALE_X);

	EditScaleZ = new CEdit();
	EditScaleZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(210, 70, 260, 100), this, ID_EDIT_SCALE_X);
}
