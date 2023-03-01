﻿// ObjectSelectorDlg.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "afxdialogex.h"
#include "ObjectSelectorDlg.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"

// ObjectSelectorDlg 대화 상자

IMPLEMENT_DYNAMIC(ObjectSelectorDlg, CDialogEx)

ObjectSelectorDlg::ObjectSelectorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ObjectSelectorDlg, pParent)
{

}

ObjectSelectorDlg::~ObjectSelectorDlg()
{
}

void ObjectSelectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_DIRECTIONAL_LIGHT, BtnDirectionalLight);
	DDX_Control(pDX, IDC_BUTTON_SPOT_LIGHT, BtnSpotLight);
	DDX_Control(pDX, IDC_BUTTON_POINT_LIGHT, BtnPointLight);
}


BEGIN_MESSAGE_MAP(ObjectSelectorDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_DIRECTIONAL_LIGHT, &ObjectSelectorDlg::OnBnClickedButtonDirectionalLight)
	ON_BN_CLICKED(IDC_BUTTON_SPOT_LIGHT, &ObjectSelectorDlg::OnBnClickedButtonSpotLight)
	ON_BN_CLICKED(IDC_BUTTON_POINT_LIGHT, &ObjectSelectorDlg::OnBnClickedButtonPointLight)
END_MESSAGE_MAP()


// ObjectSelectorDlg 메시지 처리기


BOOL ObjectSelectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void ObjectSelectorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	int x = 5;
	int y = 5;
	int width = cx - 10;
	int height = 30;

	if (BtnDirectionalLight.GetSafeHwnd() != NULL)
	{
		BtnDirectionalLight.MoveWindow(x, y, width, height);
		y += height + 5;
	}

	if (BtnSpotLight.GetSafeHwnd() != NULL)
	{
		BtnSpotLight.MoveWindow(x, y, width, height);
		y += height + 5;
	}

	if (BtnPointLight.GetSafeHwnd() != NULL)
	{
		BtnPointLight.MoveWindow(x, y, width, height);
		y += height + 5;
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void ObjectSelectorDlg::OnBnClickedButtonDirectionalLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DirectionalLight* dirLight = new DirectionalLight;
	auto lightComp = dirLight->GetComponent<DirectionalLightComponent>();
	lightComp->Color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	lightComp->Direction = Vector4(0.0f, -1.0f, 1.0f, 1.0f);
	theApp.m_TestClass->World.AddEntity(dirLight);
	theApp.Update();
}


void ObjectSelectorDlg::OnBnClickedButtonSpotLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SpotLight* spotLight = new SpotLight;
	auto lightComp2 = spotLight->GetComponent<SpotLightComponent>();
	lightComp2->Color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	lightComp2->Direction = Vector4(0.0f, -1.0f, 1.0f, 1.0f);
	lightComp2->Radius = 55.0f;
	auto transformComp = spotLight->GetComponent<TransformComponent>();
	transformComp->Translation += Vector3(10.0f, 20.0f, 10.0f);
	theApp.m_TestClass->World.AddEntity(spotLight);
	theApp.Update();
}

void ObjectSelectorDlg::OnBnClickedButtonPointLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PointLight* spotLight = new PointLight;
	auto lightComp2 = spotLight->GetComponent<PointLightComponent>();
	lightComp2->Color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	lightComp2->Direction = Vector4(0.0f, 1.0f, 1.0f, 1.0f);
	lightComp2->Radius = 25.0f;
	auto transformComp = spotLight->GetComponent<TransformComponent>();
	transformComp->Translation += Vector3(20.0f, 10.0f, 20.0f);
	theApp.m_TestClass->World.AddEntity(spotLight);
	theApp.Update();
}
