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
	/*DDX_Text(pDX, ID_EDIT_LOCATION_X, LocationX);
	DDX_Text(pDX, ID_EDIT_LOCATION_Y, LocationY);
	DDX_Text(pDX, ID_EDIT_LOCATION_Z, LocationZ);
	DDX_Text(pDX, ID_EDIT_ROTATION_X, RotationX);
	DDX_Text(pDX, ID_EDIT_ROTATION_Y, RotationY);
	DDX_Text(pDX, ID_EDIT_ROTATION_Z, RotationZ);
	DDX_Text(pDX, ID_EDIT_SCALE_X, ScaleX);
	DDX_Text(pDX, ID_EDIT_SCALE_Y, ScaleY);
	DDX_Text(pDX, ID_EDIT_SCALE_Z, ScaleZ);*/
}

BEGIN_MESSAGE_MAP(LandscapeManagementFormView, CFormView)
	ON_EN_CHANGE(ID_EDIT_LOCATION_X, &OnEditChangeLocationX)
	ON_EN_CHANGE(ID_EDIT_LOCATION_Y, &OnEditChangeLocationY)
	ON_EN_CHANGE(ID_EDIT_LOCATION_Z, &OnEditChangeLocationZ)
	ON_EN_CHANGE(ID_EDIT_ROTATION_X, &OnEditChangeRotationX)
	ON_EN_CHANGE(ID_EDIT_ROTATION_Y, &OnEditChangeRotationY)
	ON_EN_CHANGE(ID_EDIT_ROTATION_Z, &OnEditChangeRotationZ)
	ON_EN_CHANGE(ID_EDIT_SCALE_X, &OnEditChangeScaleX)
	ON_EN_CHANGE(ID_EDIT_SCALE_Y, &OnEditChangeScaleY)
	ON_EN_CHANGE(ID_EDIT_SCALE_Z, &OnEditChangeScaleZ)
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
	EditLocationX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(110, 10, 160, 40), this, ID_EDIT_LOCATION_X);
	EditLocationX->SetWindowTextW(L"0.0");
	
	EditLocationY = new CEdit();
	EditLocationY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(160, 10, 210, 40), this, ID_EDIT_LOCATION_Y);
	EditLocationY->SetWindowTextW(L"0.0");

	EditLocationZ = new CEdit(); 
	EditLocationZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(210, 10, 260, 40), this, ID_EDIT_LOCATION_Z);
	EditLocationZ->SetWindowTextW(L"0.0");

	//---------------------------
	// Rotation
	//---------------------------
	StaticRotation = new CStatic();
	StaticRotation->Create(L"Rotation", WS_CHILD | WS_VISIBLE /*| SS_CENTER*/, CRect(10, 40, 110, 70), this);

	EditRotationX = new CEdit();
	EditRotationX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(110, 40, 160, 70), this, ID_EDIT_ROTATION_X);

	EditRotationY = new CEdit();
	EditRotationY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(160, 40, 210, 70), this, ID_EDIT_ROTATION_Y);

	EditRotationZ = new CEdit();
	EditRotationZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(210, 40, 260, 70), this, ID_EDIT_ROTATION_Z);

	//---------------------------
	// Scale
	//---------------------------
	StaticScale = new CStatic();
	StaticScale->Create(L"Scale", WS_CHILD | WS_VISIBLE /*| SS_CENTER*/, CRect(10, 70, 110, 100), this);

	EditScaleX = new CEdit();
	EditScaleX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(110, 70, 160, 100), this, ID_EDIT_SCALE_X);

	EditScaleY = new CEdit();
	EditScaleY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(160, 70, 210, 100), this, ID_EDIT_SCALE_X);

	EditScaleZ = new CEdit();
	EditScaleZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(210, 70, 260, 100), this, ID_EDIT_SCALE_X);


}

void LandscapeManagementFormView::OnEditChangeLocationX()
{
	UpdateData(TRUE);
	CString strData;
	GetDlgItemText(ID_EDIT_LOCATION_X, strData);
	float locationX = _tstof(strData);
}

void LandscapeManagementFormView::OnEditChangeLocationY()
{

}

void LandscapeManagementFormView::OnEditChangeLocationZ()
{

}

void LandscapeManagementFormView::OnEditChangeRotationX()
{

}

void LandscapeManagementFormView::OnEditChangeRotationY()
{

}

void LandscapeManagementFormView::OnEditChangeRotationZ()
{

}

void LandscapeManagementFormView::OnEditChangeScaleX()
{

}

void LandscapeManagementFormView::OnEditChangeScaleY()
{

}

void LandscapeManagementFormView::OnEditChangeScaleZ()
{

}
