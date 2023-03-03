// LandscapeSplattingFormView.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "LandscapeSplattingFormView.h"
#include "Landscape.h"


// LandscapeSplattingFormView

IMPLEMENT_DYNCREATE(LandscapeSplattingFormView, CFormView)

LandscapeSplattingFormView::LandscapeSplattingFormView()
	: CFormView(IDD_LandscapeSplattingFormView)
{

}

LandscapeSplattingFormView::~LandscapeSplattingFormView()
{
}

void LandscapeSplattingFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LandscapeBaseTexture, BaseTextureComboBox);
	DDX_Control(pDX, IDC_LandscapeLayer1, Layer1TextureComboBox);
	DDX_Control(pDX, IDC_LandscapeLayer2, Layer2TextureComboBox);
	DDX_Control(pDX, IDC_LandscapeLayer3, Layer3TextureComboBox);
	DDX_Control(pDX, IDC_LandscapeLayer4, Layer4TextureComboBox);
	DDX_Control(pDX, IDC_LayerSelectCombo, LayerSelectComboBox);
}

BEGIN_MESSAGE_MAP(LandscapeSplattingFormView, CFormView)
	ON_CBN_SELCHANGE(IDC_LandscapeBaseTexture, &LandscapeSplattingFormView::OnCbnSelchangeLandscapebasetexture)
	ON_CBN_SELCHANGE(IDC_LandscapeLayer1, &LandscapeSplattingFormView::OnCbnSelchangeLandscapelayer1)
	ON_CBN_SELCHANGE(IDC_LandscapeLayer2, &LandscapeSplattingFormView::OnCbnSelchangeLandscapelayer2)
	ON_CBN_SELCHANGE(IDC_LandscapeLayer3, &LandscapeSplattingFormView::OnCbnSelchangeLandscapelayer3)
	ON_CBN_SELCHANGE(IDC_LandscapeLayer4, &LandscapeSplattingFormView::OnCbnSelchangeLandscapelayer4)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_LayerSelectCombo, &LandscapeSplattingFormView::OnCbnSelchangeLayerselectcombo)
END_MESSAGE_MAP()


// LandscapeSplattingFormView 진단

#ifdef _DEBUG
void LandscapeSplattingFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void LandscapeSplattingFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// LandscapeSplattingFormView 메시지 처리기


void LandscapeSplattingFormView::OnCbnSelchangeLandscapebasetexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (theApp.SelectedEntity == nullptr)
	{
		return;
	}

	if (theApp.SelectedEntity->has<Landscape>())
	{
		auto landscape = theApp.SelectedEntity->GetComponent<Landscape>();
		int idx = BaseTextureComboBox.GetCurSel();
		if (idx == 0)
		{
			DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(TextureFolderPath + L"Default.bmp");
			landscape->BaseTexture = pTexture;
		}
		else if(idx > 0 && idx < TextureList.size())
		{
			DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(TextureFolderPath + TextureList[idx - 1]);
			landscape->BaseTexture = pTexture;
		}
	}
}


void LandscapeSplattingFormView::OnCbnSelchangeLandscapelayer1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (theApp.SelectedEntity == nullptr)
	{
		return;
	}

	if (theApp.SelectedEntity->has<Landscape>())
	{
		auto landscape = theApp.SelectedEntity->GetComponent<Landscape>();
		int idx = Layer1TextureComboBox.GetCurSel();
		if (idx == 0)
		{
			if (!landscape->LayerTextureList.empty())
			{
				landscape->LayerTextureList[0] = landscape->BaseTexture;
			}
		}
		else if (idx > 0 && idx < TextureList.size())
		{
			if (!landscape->LayerTextureList.empty())
			{
				DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(TextureFolderPath + TextureList[idx - 1]);
				landscape->LayerTextureList[0] = pTexture;
			}
		}
	}
}


void LandscapeSplattingFormView::OnCbnSelchangeLandscapelayer2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (theApp.SelectedEntity == nullptr)
	{
		return;
	}

	if (theApp.SelectedEntity->has<Landscape>())
	{
		auto landscape = theApp.SelectedEntity->GetComponent<Landscape>();
		int idx = Layer2TextureComboBox.GetCurSel();
		if (idx == 0)
		{
			if (!landscape->LayerTextureList.empty())
			{
				landscape->LayerTextureList[1] = landscape->BaseTexture;
			}
		}
		else if (idx > 0 && idx < TextureList.size())
		{
			if (!landscape->LayerTextureList.empty())
			{
				DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(TextureFolderPath + TextureList[idx - 1]);
				landscape->LayerTextureList[1] = pTexture;
			}
		}
	}
}


void LandscapeSplattingFormView::OnCbnSelchangeLandscapelayer3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (theApp.SelectedEntity == nullptr)
	{
		return;
	}

	if (theApp.SelectedEntity->has<Landscape>())
	{
		auto landscape = theApp.SelectedEntity->GetComponent<Landscape>();
		int idx = Layer3TextureComboBox.GetCurSel();
		if (idx == 0)
		{
			if (!landscape->LayerTextureList.empty())
			{
				landscape->LayerTextureList[2] = landscape->BaseTexture;
			}
		}
		else if (idx > 0 && idx < TextureList.size())
		{
			if (!landscape->LayerTextureList.empty())
			{
				DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(TextureFolderPath + TextureList[idx - 1]);
				landscape->LayerTextureList[2] = pTexture;
			}
		}
	}
}


void LandscapeSplattingFormView::OnCbnSelchangeLandscapelayer4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (theApp.SelectedEntity == nullptr)
	{
		return;
	}

	if (theApp.SelectedEntity->has<Landscape>())
	{
		auto landscape = theApp.SelectedEntity->GetComponent<Landscape>();
		int idx = Layer4TextureComboBox.GetCurSel();
		if (idx == 0)
		{
			if (!landscape->LayerTextureList.empty())
			{
				landscape->LayerTextureList[3] = landscape->BaseTexture;
			}
		}
		else if (idx > 0 && idx < TextureList.size())
		{
			if (!landscape->LayerTextureList.empty())
			{
				DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(TextureFolderPath + TextureList[idx - 1]);
				landscape->LayerTextureList[3] = pTexture;
			}
		}
	}
}

void LandscapeSplattingFormView::GetTextureList(std::wstring _path)
{
	TextureFolderPath = _path;
	std::filesystem::path path(_path);
	for (auto& file : std::filesystem::directory_iterator(path))
	{
		std::wstring filename = file.path().filename();
		std::wstring filepath = file.path();
		std::wstring fileExtension = file.path().extension();

		if ((fileExtension == L".bmp") || (fileExtension == L".jpg"))
		{
			DXTextureManager::getInstance()->Load(filepath);
			TextureList.push_back(filename);
		}
		else
		{
			continue;
		}
	}
}

void LandscapeSplattingFormView::Initialize()
{
	if (!isInitialize)
	{
		TextureList.clear();
		BaseTextureComboBox.Clear();
		Layer1TextureComboBox.Clear();
		Layer2TextureComboBox.Clear();
		Layer3TextureComboBox.Clear();
		Layer4TextureComboBox.Clear();

		GetTextureList(L"../resource/");
		if (GetSafeHwnd())
		{
			BaseTextureComboBox.AddString(L"Default.bmp");
			Layer1TextureComboBox.AddString(L"None");
			Layer2TextureComboBox.AddString(L"None");
			Layer3TextureComboBox.AddString(L"None");
			Layer4TextureComboBox.AddString(L"None");
			for (auto& it : TextureList)
			{
				BaseTextureComboBox.AddString(it.c_str());
				Layer1TextureComboBox.AddString(it.c_str());
				Layer2TextureComboBox.AddString(it.c_str());
				Layer3TextureComboBox.AddString(it.c_str());
				Layer4TextureComboBox.AddString(it.c_str());
			}
		}

		LayerSelectComboBox.AddString(L"Layer 1");
		LayerSelectComboBox.AddString(L"Layer 2");
		LayerSelectComboBox.AddString(L"Layer 3");
		LayerSelectComboBox.AddString(L"Layer 4");

		BaseTextureComboBox.SetCurSel(0);
		Layer1TextureComboBox.SetCurSel(0);
		Layer2TextureComboBox.SetCurSel(0);
		Layer3TextureComboBox.SetCurSel(0);
		Layer4TextureComboBox.SetCurSel(0);
		LayerSelectComboBox.SetCurSel(0);
		isInitialize = true;
	}
}


int LandscapeSplattingFormView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void LandscapeSplattingFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	
}


void LandscapeSplattingFormView::OnCbnSelchangeLayerselectcombo()
{
	int idx = LayerSelectComboBox.GetCurSel();
	theApp.m_TestClass->SelectedSplattingLayer = idx;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
