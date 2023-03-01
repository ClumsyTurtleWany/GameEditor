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
	DDX_Control(pDX, IDC_FBX_FILE_LIST, FBXFileListBox);
}

BEGIN_MESSAGE_MAP(LandscapeManagementFormView, CFormView)


	ON_EN_CHANGE(IDC_COMBO_SECTION, &OnComboChangedSection)
	ON_EN_CHANGE(ID_EDIT_COMP_WIDTH, &OnEditChangedCompWidth)
	ON_EN_CHANGE(ID_EDIT_COMP_HEIGHT, &OnEditChangedCompHeight)

	ON_BN_CLICKED(IDB_BTN_BUILD_LANDSCAPE, &OnBtnClickedBuildLandscape)
	
	ON_BN_CLICKED(IDB_FBX_SELECT, &OnBtnClickedFbxSelect)

	ON_LBN_SELCHANGE(ID_STATICMESH_LIST_BOX, &OnSelChangeFbxFile)
	//ON_BN_CLICKED(IDC_BUTTON1, &LandscapeManagementFormView::OnBnClickedButton1)
	//ON_LBN_SELCHANGE(ID_STATICMESH_LIST_BOX, &LandscapeManagementFormView::OnLbnSelchangeList1)
	ON_LBN_SELCHANGE(IDC_FBX_FILE_LIST, &LandscapeManagementFormView::OnLbnSelchangeFbxFileList)
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
	//StaticLocation = new CStatic();
	//StaticLocation->Create(L"Location", WS_CHILD | WS_VISIBLE /*| SS_CENTER*/, CRect(10, 10, 110, 40), this);

	//EditLocationX = new CEdit();
	//EditLocationX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(110, 10, 160, 40), this, ID_EDIT_LOCATION_X);
	//EditLocationX->SetWindowTextW(L"0.0");
	//
	//EditLocationY = new CEdit();
	//EditLocationY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(160, 10, 210, 40), this, ID_EDIT_LOCATION_Y);
	//EditLocationY->SetWindowTextW(L"0.0");

	//EditLocationZ = new CEdit(); 
	//EditLocationZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(210, 10, 260, 40), this, ID_EDIT_LOCATION_Z);
	//EditLocationZ->SetWindowTextW(L"0.0");

	////---------------------------
	//// Rotation
	////---------------------------
	//StaticRotation = new CStatic();
	//StaticRotation->Create(L"Rotation", WS_CHILD | WS_VISIBLE /*| SS_CENTER*/, CRect(10, 40, 110, 70), this);

	//EditRotationX = new CEdit();
	//EditRotationX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(110, 40, 160, 70), this, ID_EDIT_ROTATION_X);

	//EditRotationY = new CEdit();
	//EditRotationY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(160, 40, 210, 70), this, ID_EDIT_ROTATION_Y);

	//EditRotationZ = new CEdit();
	//EditRotationZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(210, 40, 260, 70), this, ID_EDIT_ROTATION_Z);

	////---------------------------
	//// Scale
	////---------------------------
	//StaticScale = new CStatic();
	//StaticScale->Create(L"Scale", WS_CHILD | WS_VISIBLE /*| SS_CENTER*/, CRect(10, 70, 110, 100), this);

	//EditScaleX = new CEdit();
	//EditScaleX->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(110, 70, 160, 100), this, ID_EDIT_SCALE_X);

	//EditScaleY = new CEdit();
	//EditScaleY->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(160, 70, 210, 100), this, ID_EDIT_SCALE_X);

	//EditScaleZ = new CEdit();
	//EditScaleZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(210, 70, 260, 100), this, ID_EDIT_SCALE_X);

	StaticSection = new CStatic();
	StaticSection->Create(L"Section", WS_CHILD | WS_VISIBLE, CRect(10, 10, 110, 40), this);

	ComboSection = new CComboBox();
	ComboSection->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL, CRect(110, 10, 210, 40), this, IDC_COMBO_SECTION);
	ComboSection->InsertString(0, L"7 * 7");
	ComboSection->InsertString(1, L"15 * 15");

	StaticComponent = new CStatic();
	StaticComponent->Create(L"Component", WS_CHILD | WS_VISIBLE, CRect(10, 40, 110, 70), this);

	EditCompWidth = new CEdit();
	EditCompWidth->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(110, 40, 160, 70), this, ID_EDIT_COMP_WIDTH);
	EditCompWidth->SetWindowTextW(L"8");

	EditCompHeight = new CEdit();
	EditCompHeight->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, CRect(170, 40, 220, 70), this, ID_EDIT_COMP_HEIGHT);
	EditCompHeight->SetWindowTextW(L"8");

	BtnBuildLandscape = new CButton();
	BtnBuildLandscape->Create(L"Build", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(10, 80, 110, 110), this, IDB_BTN_BUILD_LANDSCAPE);
	BtnBuildLandscape->ShowWindow(SW_SHOW);


	//FbxFileListBox = new CListBox();
	//FbxFileListBox->Create(WS_CHILD | WS_VISIBLE | LBS_STANDARD | WS_HSCROLL, CRect(10, 120, 260, 310), this, ID_STATICMESH_LIST_BOX);
	FBXFileListBox.MoveWindow(10, 120, 250, 200);
	std::vector<std::wstring> FBXList;
	FBXLoader::getInstance()->GetFBXFileList(L"../resource/FBX/", FBXList);
	/*for (size_t idx = 0; idx < FBXList.size(); idx++)
	{
		ListBox->InsertString(idx, FBXList[idx].c_str());
	}*/

	/*for (auto& it : FBXList)
	{
		FbxFileListBox->AddString(it.c_str());
	}
	FbxFileListBox->SetCurSel(0);*/

	for (auto& it : FBXList)
	{
		FBXFileListBox.AddString(it.c_str());
	}
	FBXFileListBox.SetCurSel(0);

	BtnFbxSelect = new CButton();
	BtnFbxSelect->Create(L"FbxSelect", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(10, 330, 80, 360), this, IDB_FBX_SELECT);
	BtnFbxSelect->ShowWindow(SW_SHOW);
}



void LandscapeManagementFormView::OnBtnClickedFbxSelect()
{
	/*UpdateData(TRUE);
	int idx = FbxFileListBox->GetCurSel();
	if (idx < 0)
	{
		idx = 0;
	}
	CString filename;
	FbxFileListBox->GetText(idx, filename);
	theApp.m_TestClass->SelectedFilename.assign(filename.GetString());*/

	UpdateData(TRUE);
	int idx = FBXFileListBox.GetCurSel();
	if (idx < 0)
	{
		idx = 0;
	}
	CString filename;
	FBXFileListBox.GetText(idx, filename);
	theApp.m_TestClass->SelectedFilename.assign(filename.GetString());
}

void LandscapeManagementFormView::OnComboChangedSection()
{
	UpdateData();
}

void LandscapeManagementFormView::OnEditChangedCompWidth()
{
	UpdateData(TRUE);
	CString strData;
	GetDlgItemText(ID_EDIT_COMP_WIDTH, strData);
	float locationX = _tstof(strData);
}

void LandscapeManagementFormView::OnEditChangedCompHeight()
{
	UpdateData(TRUE);
	CString strData;
	GetDlgItemText(ID_EDIT_COMP_HEIGHT, strData);
	float locationX = _tstof(strData);
}

void LandscapeManagementFormView::OnBtnClickedBuildLandscape()
{
	int idx = ComboSection->GetCurSel();
	CString strWidth;
	GetDlgItemText(ID_EDIT_COMP_WIDTH, strWidth);
	CString strHeight;
	GetDlgItemText(ID_EDIT_COMP_HEIGHT, strHeight);
	
	int sectionSize = 0;
	if (idx == 0)
	{
		sectionSize = 7;
	}
	else if (idx == 1)
	{
		sectionSize = 15;
	}

	int width = _tstoi(strWidth);
	int height = _tstoi(strHeight);
	if (width > 0 && height > 0)
	{
		theApp.m_TestClass->CreateLandscape(width, height, 7);
		theApp.Update();
	}

}

void LandscapeManagementFormView::OnSelChangeFbxFile()
{
	//int idx = FbxFileListBox->GetCurSel();
}


void LandscapeManagementFormView::OnDraw(CDC* pDC)
{
	BtnFbxSelect->ShowWindow(SW_SHOW);
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void LandscapeManagementFormView::OnLbnSelchangeFbxFileList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


}
