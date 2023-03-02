// DetailDockPane.cpp: 구현 파일
//

#include "pch.h"
#include "GameEditorTest_Ver0.h"
#include "DetailDockPane.h"
#include "TransformComponent.h"
#include "LightComponent.h"
#include "Landscape.h"


// DetailDockPane

IMPLEMENT_DYNAMIC(DetailDockPane, CDockablePane)

DetailDockPane::DetailDockPane()
{

}

DetailDockPane::~DetailDockPane()
{
}


BEGIN_MESSAGE_MAP(DetailDockPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &DetailDockPane::OnPropertyChanged)
END_MESSAGE_MAP()



// DetailDockPane 메시지 처리기




int DetailDockPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CRect rc;
	GetClientRect(rc);

	if (!m_PropList.Create(WS_VISIBLE | WS_CHILD, rc, this, IDC_DETAIL_TREE))
	{
		return -1;
	}

	return 0;
}


void DetailDockPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}
	
	CRect rectClient;
	GetClientRect(rectClient);

	m_PropList.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}


void DetailDockPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_PropList.SetFocus();
}


void DetailDockPane::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetFont();
}

void DetailDockPane::SetFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_PropList.SetFont(&m_fntPropList);
}

void DetailDockPane::UpdateDetail(ECS::Entity* entity)
{
	if (TargetEntity == entity)
	{
		return;
	}

	TargetEntity = entity;

	SetFont();

	m_PropList.RemoveAll();
	m_PropList.EnableHeaderCtrl(FALSE);
	m_PropList.EnableDescriptionArea();
	m_PropList.SetVSDotNetLook();
	m_PropList.MarkModifiedProperties();
	if (TargetEntity != nullptr)
	{
		if (entity->has<TransformComponent>())
		{
			auto comp = entity->GetComponent<TransformComponent>();
			CMFCPropertyGridProperty* transformProp = new CMFCPropertyGridProperty(_T("Transform"));
			m_PropList.AddProperty(transformProp);

			//transformProp->AddSubItem(new CMFCPropertyGridProperty)
			CMFCPropertyGridProperty* translationProp = new CMFCPropertyGridProperty(_T("Translation"), 0, TRUE);
			CMFCPropertyGridProperty* tx = new CMFCPropertyGridProperty(_T("X"), (_variant_t)(float)comp->Translation.x, _T("Translation X"));
			CMFCPropertyGridProperty* ty = new CMFCPropertyGridProperty(_T("Y"), (_variant_t)(float)comp->Translation.y, _T("Translation Y"));
			CMFCPropertyGridProperty* tz = new CMFCPropertyGridProperty(_T("Z"), (_variant_t)(float)comp->Translation.z, _T("Translation Z"));
			tx->SetData(1);
			ty->SetData(2);
			tz->SetData(3);
			translationProp->AddSubItem(tx);
			translationProp->AddSubItem(ty);
			translationProp->AddSubItem(tz);
			m_PropList.AddProperty(translationProp);

			CMFCPropertyGridProperty* rotationProp = new CMFCPropertyGridProperty(_T("Rotation"), 0, TRUE);
			CMFCPropertyGridProperty* rx = new CMFCPropertyGridProperty(_T("X"), (_variant_t)(float)comp->Rotation.x, _T("Rotation X"));
			CMFCPropertyGridProperty* ry = new CMFCPropertyGridProperty(_T("Y"), (_variant_t)(float)comp->Rotation.y, _T("Rotation Y"));
			CMFCPropertyGridProperty* rz = new CMFCPropertyGridProperty(_T("Z"), (_variant_t)(float)comp->Rotation.z, _T("Rotation Z"));
			rx->SetData(4);
			ry->SetData(5);
			rz->SetData(6);
			rotationProp->AddSubItem(rx);
			rotationProp->AddSubItem(ry);
			rotationProp->AddSubItem(rz);
			m_PropList.AddProperty(rotationProp);

			CMFCPropertyGridProperty* scaleProp = new CMFCPropertyGridProperty(_T("Scale"), 0, TRUE);
			CMFCPropertyGridProperty* sx = new CMFCPropertyGridProperty(_T("X"), (_variant_t)(float)comp->Scale.x, _T("Scale X"));
			CMFCPropertyGridProperty* sy = new CMFCPropertyGridProperty(_T("Y"), (_variant_t)(float)comp->Scale.y, _T("Scale Y"));
			CMFCPropertyGridProperty* sz = new CMFCPropertyGridProperty(_T("Z"), (_variant_t)(float)comp->Scale.z, _T("Scale Z"));
			sx->SetData(7);
			sy->SetData(8);
			sz->SetData(9);
			scaleProp->AddSubItem(sx);
			scaleProp->AddSubItem(sy);
			scaleProp->AddSubItem(sz);
			m_PropList.AddProperty(scaleProp);
		}

		if (entity->has<DirectionalLightComponent>())
		{
			auto comp = entity->GetComponent<DirectionalLightComponent>();
			CMFCPropertyGridProperty* lightProp = new CMFCPropertyGridProperty(_T("Directional Light"));
			m_PropList.AddProperty(lightProp);
			
			CMFCPropertyGridProperty* colorProp = new CMFCPropertyGridProperty(_T("Color"), 0, TRUE);
			CMFCPropertyGridProperty* r = new CMFCPropertyGridProperty(_T("R"), (_variant_t)(float)comp->Color.x, _T("Color R"));
			CMFCPropertyGridProperty* g = new CMFCPropertyGridProperty(_T("G"), (_variant_t)(float)comp->Color.y, _T("Color G"));
			CMFCPropertyGridProperty* b = new CMFCPropertyGridProperty(_T("B"), (_variant_t)(float)comp->Color.z, _T("Color B"));
			CMFCPropertyGridProperty* w = new CMFCPropertyGridProperty(_T("W"), (_variant_t)(float)comp->Color.w, _T("Alpha W"));
			r->SetData(10);
			g->SetData(11);
			b->SetData(12);
			w->SetData(13);
			colorProp->AddSubItem(r);
			colorProp->AddSubItem(g);
			colorProp->AddSubItem(b);
			colorProp->AddSubItem(w);
			m_PropList.AddProperty(colorProp);

			CMFCPropertyGridProperty* dirProp = new CMFCPropertyGridProperty(_T("Direction"), 0, TRUE);
			CMFCPropertyGridProperty* x = new CMFCPropertyGridProperty(_T("X"), (_variant_t)(float)comp->Direction.x, _T("Direction X"));
			CMFCPropertyGridProperty* y = new CMFCPropertyGridProperty(_T("Y"), (_variant_t)(float)comp->Direction.y, _T("Direction Y"));
			CMFCPropertyGridProperty* z = new CMFCPropertyGridProperty(_T("Z"), (_variant_t)(float)comp->Direction.z, _T("Direction Z"));
			x->SetData(14);
			y->SetData(15);
			z->SetData(16);
			dirProp->AddSubItem(x);
			dirProp->AddSubItem(y);
			dirProp->AddSubItem(z);
			m_PropList.AddProperty(dirProp);
		}

		if (entity->has<PointLightComponent>())
		{
			auto comp = entity->GetComponent<PointLightComponent>();
			CMFCPropertyGridProperty* lightProp = new CMFCPropertyGridProperty(_T("Point Light"));
			m_PropList.AddProperty(lightProp);

			CMFCPropertyGridProperty* colorProp = new CMFCPropertyGridProperty(_T("Color"), 0, TRUE);
			CMFCPropertyGridProperty* r = new CMFCPropertyGridProperty(_T("R"), (_variant_t)(float)comp->Color.x, _T("Color R"));
			CMFCPropertyGridProperty* g = new CMFCPropertyGridProperty(_T("G"), (_variant_t)(float)comp->Color.y, _T("Color G"));
			CMFCPropertyGridProperty* b = new CMFCPropertyGridProperty(_T("B"), (_variant_t)(float)comp->Color.z, _T("Color B"));
			CMFCPropertyGridProperty* w = new CMFCPropertyGridProperty(_T("W"), (_variant_t)(float)comp->Color.w, _T("Alpha W"));
			r->SetData(20);
			g->SetData(21);
			b->SetData(22);
			w->SetData(23);
			colorProp->AddSubItem(r);
			colorProp->AddSubItem(g);
			colorProp->AddSubItem(b);
			colorProp->AddSubItem(w);
			m_PropList.AddProperty(colorProp);

			CMFCPropertyGridProperty* dirProp = new CMFCPropertyGridProperty(_T("Direction"), 0, TRUE);
			CMFCPropertyGridProperty* x = new CMFCPropertyGridProperty(_T("X"), (_variant_t)(float)comp->Direction.x, _T("Direction X"));
			CMFCPropertyGridProperty* y = new CMFCPropertyGridProperty(_T("Y"), (_variant_t)(float)comp->Direction.y, _T("Direction Y"));
			CMFCPropertyGridProperty* z = new CMFCPropertyGridProperty(_T("Z"), (_variant_t)(float)comp->Direction.z, _T("Direction Z"));
			x->SetData(24);
			y->SetData(25);
			z->SetData(26);
			dirProp->AddSubItem(x);
			dirProp->AddSubItem(y);
			dirProp->AddSubItem(z);
			m_PropList.AddProperty(dirProp);

			CMFCPropertyGridProperty* radiusProp = new CMFCPropertyGridProperty(_T("Radius"), 0, TRUE);
			CMFCPropertyGridProperty* radius = new CMFCPropertyGridProperty(_T("Radius"), (_variant_t)(float)comp->Radius, _T("Radius"));
			radius->SetData(27);
			radiusProp->AddSubItem(radius);
			m_PropList.AddProperty(radiusProp);
		}

		if (entity->has<SpotLightComponent>())
		{
			auto comp = entity->GetComponent<SpotLightComponent>();
			CMFCPropertyGridProperty* lightProp = new CMFCPropertyGridProperty(_T("Spot Light"));
			m_PropList.AddProperty(lightProp);

			CMFCPropertyGridProperty* colorProp = new CMFCPropertyGridProperty(_T("Color"), 0, TRUE);
			CMFCPropertyGridProperty* r = new CMFCPropertyGridProperty(_T("R"), (_variant_t)(float)comp->Color.x, _T("Color R"));
			CMFCPropertyGridProperty* g = new CMFCPropertyGridProperty(_T("G"), (_variant_t)(float)comp->Color.y, _T("Color G"));
			CMFCPropertyGridProperty* b = new CMFCPropertyGridProperty(_T("B"), (_variant_t)(float)comp->Color.z, _T("Color B"));
			CMFCPropertyGridProperty* w = new CMFCPropertyGridProperty(_T("W"), (_variant_t)(float)comp->Color.w, _T("Alpha W"));
			r->SetData(30);
			g->SetData(31);
			b->SetData(32);
			w->SetData(33);
			colorProp->AddSubItem(r);
			colorProp->AddSubItem(g);
			colorProp->AddSubItem(b);
			colorProp->AddSubItem(w);
			m_PropList.AddProperty(colorProp);

			CMFCPropertyGridProperty* dirProp = new CMFCPropertyGridProperty(_T("Direction"), 0, TRUE);
			CMFCPropertyGridProperty* x = new CMFCPropertyGridProperty(_T("X"), (_variant_t)(float)comp->Direction.x, _T("Direction X"));
			CMFCPropertyGridProperty* y = new CMFCPropertyGridProperty(_T("Y"), (_variant_t)(float)comp->Direction.y, _T("Direction Y"));
			CMFCPropertyGridProperty* z = new CMFCPropertyGridProperty(_T("Z"), (_variant_t)(float)comp->Direction.z, _T("Direction Z"));
			x->SetData(34);
			y->SetData(35);
			z->SetData(36);
			dirProp->AddSubItem(x);
			dirProp->AddSubItem(y);
			dirProp->AddSubItem(z);
			m_PropList.AddProperty(dirProp);

			CMFCPropertyGridProperty* radiusProp = new CMFCPropertyGridProperty(_T("Radius"), 0, TRUE);
			CMFCPropertyGridProperty* radius = new CMFCPropertyGridProperty(_T("Radius"), (_variant_t)(float)comp->Radius, _T("Radius"));
			radius->SetData(37);
			radiusProp->AddSubItem(radius);
			m_PropList.AddProperty(radiusProp);
		}

		if (entity->has<Landscape>())
		{
			auto comp = entity->GetComponent<SpotLightComponent>();
			CMFCPropertyGridProperty* textureProp = new CMFCPropertyGridProperty(_T("Texture"));
			textureProp->AddSubItem(new CMFCPropertyGridProperty(_T("Layer Enable"), (_variant_t)false, _T("Layer Enable Flag.")));
			m_PropList.AddProperty(textureProp);

		}
	}

	m_PropList.AdjustLayout();
	m_PropList.RedrawWindow();
}

LRESULT DetailDockPane::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	CMFCPropertyGridProperty* pProp = NULL;
	pProp = (CMFCPropertyGridProperty*)lp;

	if (!pProp)
		return 0L;

	_variant_t var = pProp->GetValue();

	int id = pProp->GetData();

	CString str;
	str = pProp->GetName();


	switch (var.vt)
	{
	case VT_UINT:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.uintVal);
	case VT_INT:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.intVal);
	case VT_I2:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.iVal);
	case VT_I4:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.lVal);
		break;
	case VT_R4:
		str.Format(_T("Name(ID:%d):%s, Value:%f"), id, str, var.fltVal);
		break;
	case VT_R8:
		str.Format(_T("Name(ID:%d):%s, Value:%f"), id, str, var.dblVal);
		break;
	case VT_BSTR:
		str.Format(_T("Name(ID:%d):%s, Value:%s"), id, str, var.bstrVal);
		break;
	case VT_BOOL:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.boolVal);
		break;
	}

	//AfxMessageBox(str);
	if (id < 10)
	{
		auto comp = TargetEntity->GetComponent<TransformComponent>();

		switch (id)
		{
		case 1:
		{
			comp->Translation.x = var.fltVal;
		}
		break;
		case 2:
		{
			comp->Translation.y = var.fltVal;
		}
		break;
		case 3:
		{
			comp->Translation.z = var.fltVal;
		}
		break;
		case 4:
		{
			comp->Rotation.x = var.fltVal;
		}
		break;
		case 5:
		{
			comp->Rotation.y = var.fltVal;
		}
		break;
		case 6:
		{
			comp->Rotation.z = var.fltVal;
		}
		break;
		case 7:
		{
			comp->Scale.x = var.fltVal;
		}
		break;
		case 8:
		{
			comp->Scale.y = var.fltVal;
		}
		break;
		case 9:
		{
			comp->Scale.z = var.fltVal;
		}
		break;
		}
	}
	else if (id >= 10 && id < 20)
	{
		auto comp = TargetEntity->GetComponent<DirectionalLightComponent>();
		switch (id)
		{
		case 10:
		{
			comp->Color.x = var.fltVal;
		}
		break;
		case 11:
		{
			comp->Color.y = var.fltVal;
		}
		break;
		case 12:
		{
			comp->Color.z = var.fltVal;
		}
		break;
		case 13:
		{
			comp->Color.w = var.fltVal;
		}
		break;
		case 14:
		{
			comp->Direction.x = var.fltVal;
		}
		break;
		case 15:
		{
			comp->Direction.y = var.fltVal;
		}
		break;
		case 16:
		{
			comp->Direction.z = var.fltVal;
		}
		break;
		}
	}
	else if (id >= 20 && id < 30)
	{
		auto comp = TargetEntity->GetComponent<PointLightComponent>();
		switch (id)
		{
		case 20:
		{
			comp->Color.x = var.fltVal;
		}
		break;
		case 21:
		{
			comp->Color.y = var.fltVal;
		}
		break;
		case 22:
		{
			comp->Color.z = var.fltVal;
		}
		break;
		case 23:
		{
			comp->Color.w = var.fltVal;
		}
		break;
		case 24:
		{
			comp->Direction.x = var.fltVal;
		}
		break;
		case 25:
		{
			comp->Direction.y = var.fltVal;
		}
		break;
		case 26:
		{
			comp->Direction.z = var.fltVal;
		}
		break;
		case 27:
		{
			comp->Radius = var.fltVal;
		}
		break;
		}
	}
	else if (id >= 30 && id < 40)
	{
	auto comp = TargetEntity->GetComponent<SpotLightComponent>();
	switch (id)
	{
	case 30:
	{
		comp->Color.x = var.fltVal;
	}
	break;
	case 31:
	{
		comp->Color.y = var.fltVal;
	}
	break;
	case 32:
	{
		comp->Color.z = var.fltVal;
	}
	break;
	case 33:
	{
		comp->Color.w = var.fltVal;
	}
	break;
	case 34:
	{
		comp->Direction.x = var.fltVal;
	}
	break;
	case 35:
	{
		comp->Direction.y = var.fltVal;
	}
	break;
	case 36:
	{
		comp->Direction.z = var.fltVal;
	}
	break;
	case 37:
	{
		comp->Radius = var.fltVal;
	}
	break;
	}
	}


	return 0L;
}
