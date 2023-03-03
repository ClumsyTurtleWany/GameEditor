#pragma once



// LandscapeSplattingFormView 폼 보기

class LandscapeSplattingFormView : public CFormView
{
	DECLARE_DYNCREATE(LandscapeSplattingFormView)

protected:
	LandscapeSplattingFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LandscapeSplattingFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LandscapeSplattingFormView };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox BaseTextureComboBox;
	CComboBox Layer1TextureComboBox;
	CComboBox Layer2TextureComboBox;
	CComboBox Layer3TextureComboBox;
	CComboBox Layer4TextureComboBox;
	CComboBox LayerSelectComboBox;
	std::vector<std::wstring> TextureList;
	std::wstring TextureFolderPath;

	bool isInitialize = false;

	afx_msg void OnCbnSelchangeLandscapebasetexture();
	afx_msg void OnCbnSelchangeLandscapelayer1();
	afx_msg void OnCbnSelchangeLandscapelayer2();
	afx_msg void OnCbnSelchangeLandscapelayer3();
	afx_msg void OnCbnSelchangeLandscapelayer4();
	afx_msg void OnCbnSelchangeLayerselectcombo();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();

	void GetTextureList(std::wstring _path);
	void Initialize();
};


