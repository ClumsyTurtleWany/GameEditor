// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// GameEditorTest_Ver0View.cpp: CGameEditorTestVer0View 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "GameEditorTest_Ver0.h"
#endif

#include "GameEditorTest_Ver0Doc.h"
#include "GameEditorTest_Ver0View.h"
#include "DebugCamera.h"
//#include "Landscape.h"
//#include "Actor.h"
//#include "FBXLoader.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGameEditorTestVer0View

IMPLEMENT_DYNCREATE(CGameEditorTestVer0View, CView)

BEGIN_MESSAGE_MAP(CGameEditorTestVer0View, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CGameEditorTestVer0View 생성/소멸

CGameEditorTestVer0View::CGameEditorTestVer0View() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CGameEditorTestVer0View::~CGameEditorTestVer0View()
{
}

BOOL CGameEditorTestVer0View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CGameEditorTestVer0View 그리기

void CGameEditorTestVer0View::OnDraw(CDC* /*pDC*/)
{
	CGameEditorTestVer0Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}



void CGameEditorTestVer0View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGameEditorTestVer0View 진단

#ifdef _DEBUG
void CGameEditorTestVer0View::AssertValid() const
{
	CView::AssertValid();
}

void CGameEditorTestVer0View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGameEditorTestVer0Doc* CGameEditorTestVer0View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGameEditorTestVer0Doc)));
	return (CGameEditorTestVer0Doc*)m_pDocument;
}
#endif //_DEBUG


// CGameEditorTestVer0View 메시지 처리기


void CGameEditorTestVer0View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	// 여기가 실질적인 뷰포트 사이즈
	int x = cx;
	int y = cy;

	HWND hWnd = GetSafeHwnd();

	if (theApp.m_TestClass != nullptr)
	{
		if (cx > 0 && cy > 0)
		{
			float aspectRatio = static_cast<float>(cx) / static_cast<float>(cy);
			float targetRatio = 16.0f / 9.0f;
			if (aspectRatio < targetRatio)
			{
				float width = static_cast<float>(cy) * targetRatio;
				theApp.m_TestClass->Resize(0, 0, width, cy);
			}
			else if (aspectRatio > targetRatio)
			{
				float height = static_cast<float>(cx) / targetRatio;
				theApp.m_TestClass->Resize(0, 0, cx, height);
			}
			else
			{
				theApp.m_TestClass->Resize(0, 0, cx, cy);
			}
		}
	}
	int a = 0;
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CGameEditorTestVer0View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (RbtnDown)
	{
		Camera* camera = theApp.m_TestClass->MainCamera;
		float time = Timer::getInstance()->secondPerFrame;
		CPoint offset = point - PrevMousePoint;
		camera->Yaw += offset.x * time;
		camera->Pitch += offset.y * time;

		PrevMousePoint = point;
	}

	CView::OnMouseMove(nFlags, point);
}

BOOL CGameEditorTestVer0View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	Camera* camera = theApp.m_TestClass->MainCamera;
	camera->Pos += camera->Look * zDelta * 0.01f;

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CGameEditorTestVer0View::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	RbtnDown = true;
	PrevMousePoint = point;

	CView::OnRButtonDown(nFlags, point);
}

void CGameEditorTestVer0View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	RbtnDown = false;
	
	//if (!theApp.m_TestClass->SelectedFilename.empty())
	//{
	//	for (auto& it : theApp.m_TestClass->World.GetEntities<Landscape>())
	//	{
	//		auto landscape = it->GetComponent<Landscape>();
	//		for (auto& comp : landscape->Components)
	//		{
	//			for (auto& face : comp.Faces)
	//			{
	//				if (theApp.m_TestClass->Picker.CheckPick(face.V0.Pos, face.V1.Pos, face.V2.Pos))
	//				{
	//					auto mesh = theApp.m_TestClass->StaticMeshMap.find(theApp.m_TestClass->SelectedFilename);
	//					if (mesh != theApp.m_TestClass->StaticMeshMap.end())
	//					{
	//						StaticMeshComponent* meshComp = mesh->second.get();
	//						Actor* NewActor = new Actor;
	//						auto staticMeshComp = NewActor->AddComponent<StaticMeshComponent>();
	//						staticMeshComp->Meshes.assign(meshComp->Meshes.begin(), meshComp->Meshes.end());
	//						for (auto& it : staticMeshComp->Meshes)
	//						{
	//							it.SetContext(theApp.m_TestClass->GetContext());
	//						}
	//						staticMeshComp->SetContext(theApp.m_TestClass->GetContext());

	//						auto transform = NewActor->GetComponent<TransformComponent>();
	//						transform->Translation = theApp.m_TestClass->Picker.IntercetionPosition;

	//						theApp.m_TestClass->World.AddEntity(NewActor);
	//					}
	//					else
	//					{
	//						std::unique_ptr<StaticMeshComponent> staticMesh = std::make_unique<StaticMeshComponent>();
	//						if (FBXLoader::getInstance()->Load(L"../resource/FBX/" + theApp.m_TestClass->SelectedFilename, staticMesh.get()))
	//						{
	//							theApp.m_TestClass->StaticMeshMap.insert(std::make_pair(theApp.m_TestClass->SelectedFilename, std::move(staticMesh)));
	//						}

	//						auto mesh = theApp.m_TestClass->StaticMeshMap.find(theApp.m_TestClass->SelectedFilename);
	//						if (mesh != theApp.m_TestClass->StaticMeshMap.end())
	//						{
	//							StaticMeshComponent* meshComp = mesh->second.get();
	//							Actor* NewActor = new Actor;
	//							auto staticMeshComp = NewActor->AddComponent<StaticMeshComponent>();
	//							staticMeshComp->Meshes.assign(meshComp->Meshes.begin(), meshComp->Meshes.end());
	//							//staticMeshComp = meshComp;
	//							for (auto& it : staticMeshComp->Meshes)
	//							{
	//								it.SetContext(theApp.m_TestClass->GetContext());
	//							}
	//							staticMeshComp->SetContext(theApp.m_TestClass->GetContext());

	//							auto transform = NewActor->GetComponent<TransformComponent>();
	//							transform->Translation = theApp.m_TestClass->Picker.IntercetionPosition;

	//							theApp.m_TestClass->World.AddEntity(NewActor);
	//						}
	//					}

	//					theApp.Update();
	//				}
	//			}

	//		}

	//	}
	//}

	



	//ClientToScreen(&point);
	//OnContextMenu(this, point);
}

void CGameEditorTestVer0View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	LbtnDown = true;
	PrevMousePoint = point;

	if (theApp.CurrentEditorMode == EditorMode::Selector)
	{
		if (theApp.m_TestClass->AddSelectedEntity())
		{
			theApp.m_TestClass->SelectedFilename.clear();
			theApp.Update();
		}
	}
	else if (theApp.CurrentEditorMode == EditorMode::Sculpting)
	{
		theApp.m_TestClass->Sculpting();
	}
	else if (theApp.CurrentEditorMode == EditorMode::Splatting)
	{
		theApp.m_TestClass->Splatting();
	}

	CView::OnLButtonDown(nFlags, point);
}

void CGameEditorTestVer0View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	LbtnDown = false;

	CView::OnLButtonUp(nFlags, point);
}

void CGameEditorTestVer0View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nChar)
	{
	case 36: // Home
	{
		Camera* camera = theApp.m_TestClass->MainCamera;
		camera->Pos = Vector3(0.0f, 0.0f, -10.0f);
		camera->Yaw = 0.0f;
		camera->Pitch = 0.0f;
		camera->Roll = 0.0f;
	}
	break;
	}

	if (RbtnDown || LbtnDown)
	{
		Camera* camera = theApp.m_TestClass->MainCamera;
		float speed = 100.0f;
		float time = Timer::getInstance()->secondPerFrame;
		switch (nChar)
		{
		case 87: // W
		{
			camera->Pos += camera->Look * speed * time;
		}
		break;
		case 119: // w
		{
			camera->Pos += camera->Look * speed * time;
		}
		break;
		case 83: // S
		{
			camera->Pos -= camera->Look * speed * time;
		}
		break;
		case 115: // s
		{
			camera->Pos -= camera->Look * speed * time;
		}
		break;

		}
	}
	else
	{
		
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}