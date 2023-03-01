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

// MapEditorView.cpp: CMapEditorView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MapEditor.h"
#endif

#include "MapEditorDoc.h"
#include "MapEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMapEditorView

IMPLEMENT_DYNCREATE(CMapEditorView, CView)

BEGIN_MESSAGE_MAP(CMapEditorView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CMapEditorView 생성/소멸

CMapEditorView::CMapEditorView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMapEditorView::~CMapEditorView()
{
}

BOOL CMapEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMapEditorView 그리기

void CMapEditorView::OnDraw(CDC* /*pDC*/)
{
	CMapEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CMapEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	RbtnDown = false;
	if (theApp.Core->AddSelectedEntity())
	{
		theApp.Core->SelectedFilename.clear();
		theApp.Update();
	}

	//ClientToScreen(&point);
	//OnContextMenu(this, point);
}

void CMapEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMapEditorView 진단

#ifdef _DEBUG
void CMapEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CMapEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMapEditorDoc* CMapEditorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapEditorDoc)));
	return (CMapEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMapEditorView 메시지 처리기


void CMapEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int x = cx;
	int y = cy;

	HWND hWnd = GetSafeHwnd();

	if (theApp.Core != nullptr)
	{
		if (cx > 0 && cy > 0)
		{
			float aspectRatio = static_cast<float>(cx) / static_cast<float>(cy);
			float targetRatio = 16.0f / 9.0f;
			if (aspectRatio < targetRatio)
			{
				float width = static_cast<float>(cy) * targetRatio;
				theApp.Core->Resize(0, 0, width, cy);
			}
			else if (aspectRatio > targetRatio)
			{
				float height = static_cast<float>(cx) / targetRatio;
				theApp.Core->Resize(0, 0, cx, height);
			}
			else
			{
				theApp.Core->Resize(0, 0, cx, cy);
			}
		}
	}
}


void CMapEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (RbtnDown || LbtnDown)
	{
		Camera* camera = theApp.Core->GetDebugCamera();
		float time = Timer::GetInstance()->SecondPerFrame;
		CPoint offset = point - PrevMousePoint;
		camera->Yaw += offset.x * time;
		camera->Pitch += offset.y * time;

		PrevMousePoint = point;
	}

	CView::OnMouseMove(nFlags, point);
}


BOOL CMapEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	DebugCamera* camera = theApp.m_TestClass->GetDebugCamera();
	camera->Pos += camera->Look * zDelta * 0.01f;

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CMapEditorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	RbtnDown = true;
	PrevMousePoint = point;

	CView::OnRButtonDown(nFlags, point);
}


void CMapEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	LbtnDown = true;
	PrevMousePoint = point;

	CView::OnLButtonDown(nFlags, point);
}


void CMapEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	LbtnDown = false;

	CView::OnLButtonUp(nFlags, point);
}


void CMapEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nChar)
	{
	case 36: // Home
	{
		DebugCamera* camera = theApp.m_TestClass->GetDebugCamera();
		camera->Pos = Vector3(0.0f, 0.0f, -10.0f);
		camera->Yaw = 0.0f;
		camera->Pitch = 0.0f;
		camera->Roll = 0.0f;
	}
	break;
	}

	if (RbtnDown || LbtnDown)
	{
		DebugCamera* camera = theApp.m_TestClass->GetDebugCamera();
		camera->Speed = 1000.0f;
		float time = Timer::GetInstance()->SecondPerFrame;
		switch (nChar)
		{
		case 87: // W
		{
			camera->Pos += camera->Look * camera->Speed * time;
		}
		break;
		case 119: // w
		{
			camera->Pos += camera->Look * camera->Speed * time;
		}
		break;
		case 83: // S
		{
			camera->Pos -= camera->Look * camera->Speed * time;
		}
		break;
		case 115: // s
		{
			camera->Pos -= camera->Look * camera->Speed * time;
		}
		break;

		}
	}
	else
	{

	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
