#include "WidgetComponent.h"
#include "Input.hpp"

WidgetComponent::WidgetComponent()
{
	Context = DXDevice::g_pImmediateContext;
	Initialize();
}

WidgetComponent::~WidgetComponent()
{
}

bool WidgetComponent::Initialize()
{
	return true;
}


bool WidgetComponent::Render()
{
	// 여기서 대충 내할거하면될듯
	for (auto obj : ObjList) 
	{
		if (obj->m_bIsDead != true) 
		{
			obj->Frame();
			obj->Render(); 
		}
	}
	return true;
}

WidgetObject* WidgetComponent::FindObj(std::wstring ObjName)
{
	if (ObjMap.find(ObjName) == ObjMap.end()) { return nullptr; }
	else return ObjMap.find(ObjName)->second;
}

void WidgetComponent::AddObj(std::wstring ObjName, WidgetObject* wo)
{
	ObjList.push_back(wo);
	ObjMap.insert(std::make_pair(ObjName, wo));
}


void WidgetComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.Mat = TransformData.Mat.Transpose();
}
