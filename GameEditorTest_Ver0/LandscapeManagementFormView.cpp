#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "LandscapeManagementFormView.h"
// LandscapeManagementFormView

IMPLEMENT_DYNCREATE(LandscapeManagementFormView, CFormView)

LandscapeManagementFormView::LandscapeManagementFormView()
	: CFormView(IDD_LandscapeManagementFormView)
	, ComponentHeight(0)
	, ComponentWidth(0)
{

}
LandscapeManagementFormView::~LandscapeManagementFormView()
{
}

void LandscapeManagementFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LANDSCAPE_COMPONENT_WIDTH, EditComponentWidth);
	DDX_Control(pDX, IDC_LANDSCAPE_COMPONENT_HEIGHT, EditComponentHeight);
	DDX_Control(pDX, IDC_BTN_CREATE_LANDSCAPE, BtnCreateLandscape);
	DDX_Text(pDX, IDC_LANDSCAPE_COMPONENT_HEIGHT, ComponentHeight);
	DDV_MinMaxInt(pDX, ComponentHeight, 0, 100);
	DDX_Text(pDX, IDC_LANDSCAPE_COMPONENT_WIDTH, ComponentWidth);
	DDV_MinMaxInt(pDX, ComponentWidth, 0, 100);
	DDX_Control(pDX, IDC_COMBO_LANDSCAPE_SECTION_SIZE, SectionSizeComboBox);
}

BEGIN_MESSAGE_MAP(LandscapeManagementFormView, CFormView)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_LANDSCAPE_COMPONENT_WIDTH, &LandscapeManagementFormView::OnEnChangeLandscapeComponentWidth)
	ON_EN_CHANGE(IDC_LANDSCAPE_COMPONENT_HEIGHT, &LandscapeManagementFormView::OnEnChangeLandscapeComponentHeight)
	ON_BN_CLICKED(IDC_BTN_CREATE_LANDSCAPE, &LandscapeManagementFormView::OnBnClickedBtnCreateLandscape)
	ON_CBN_SELCHANGE(IDC_COMBO_LANDSCAPE_SECTION_SIZE, &LandscapeManagementFormView::OnCbnSelchangeComboLandscapeSectionSize)
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

	
}


void LandscapeManagementFormView::OnDraw(CDC* pDC)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void LandscapeManagementFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void LandscapeManagementFormView::OnEnChangeLandscapeComponentWidth()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
}


void LandscapeManagementFormView::OnEnChangeLandscapeComponentHeight()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
}


void LandscapeManagementFormView::OnBnClickedBtnCreateLandscape()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int sectionSize = 7;
	if (SectionSizeMode == 0)
	{
		sectionSize = 7;
	}
	else if (SectionSizeMode == 1)
	{
		sectionSize = 15;
	}
	else if (SectionSizeMode == 2)
	{
		sectionSize = 31;
	}
	else if (SectionSizeMode == 3)
	{
		sectionSize = 63;
	}

	if (ComponentWidth > 0 && ComponentHeight > 0)
	{
		theApp.m_TestClass->CreateLandscape(ComponentWidth, ComponentHeight, sectionSize);
		theApp.Update();
	}
}

void LandscapeManagementFormView::Initialize()
{
	SectionSizeComboBox.AddString(L"7 X 7");
	SectionSizeComboBox.AddString(L"15 X 15");
	SectionSizeComboBox.AddString(L"31 X 31");
	SectionSizeComboBox.AddString(L"63 X 63");
	SectionSizeComboBox.SetCurSel(0);

	ComponentWidth = 8;
	ComponentHeight = 8;
	
	UpdateData(FALSE);
}


void LandscapeManagementFormView::OnCbnSelchangeComboLandscapeSectionSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SectionSizeMode = SectionSizeComboBox.GetCurSel();
}
