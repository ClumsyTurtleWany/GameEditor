#include "Actor.h"
#include "BoundingBoxComponent.h"
#include "Camera.h"
#include "CameraArmComponent.h"
#include "ArrowComponent.h"
#include "AnimationComponent.h"

Actor::Actor()
{
	this->Name = L"Actor"; // +std::to_wstring(this->ID);
	Transform = this->AddComponent<TransformComponent>();
}

Actor::~Actor()
{

}

bool Actor::Save(std::string filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//루트 노드 추가
	std::string name(Name.begin(), Name.end());
	TiXmlElement* pRoot = new TiXmlElement(name.c_str());
	doc.LinkEndChild(pRoot);

	auto transform = GetComponent<TransformComponent>();
	if (transform != nullptr)
	{
		transform->WriteXML(pRoot);
	}
	
	auto movement = GetComponent<MovementComponent>();
	if (movement != nullptr)
	{
		movement->WriteXML(pRoot);
	}

	auto boundbox = GetComponent<BoundingBoxComponent>();
	if (boundbox != nullptr)
	{
		boundbox->WriteXML(pRoot);
	}

	auto camera = GetComponent<Camera>();
	if (camera != nullptr)
	{
		camera->WriteXML(pRoot);
	}

	auto cameraArm = GetComponent<CameraArmComponent>();
	if (cameraArm != nullptr)
	{
		cameraArm->WriteXML(pRoot);
	}

	auto arrow = GetComponent<ArrowComponent>();
	if (arrow != nullptr)
	{
		arrow->WriteXML(pRoot);
	}

	auto staticmesh = GetComponent<StaticMeshComponent>();
	if (staticmesh != nullptr)
	{
		staticmesh->WriteXML(pRoot);
	}

	auto skeletalmesh = GetComponent<SkeletalMeshComponent>();
	if (skeletalmesh != nullptr)
	{
		skeletalmesh->WriteXML(pRoot);
	}

	auto anim = GetComponent<AnimationComponent>();
	if (anim != nullptr)
	{
		anim->WriteXML(pRoot);
	}

	return doc.SaveFile(filename.c_str());
}

bool Actor::Load(std::string filename)
{
	TiXmlDocument doc;
	std::string readfile(filename.begin(), filename.end());
	if (!doc.LoadFile(readfile.c_str()))
	{
		return false;
	}

	TiXmlElement* root = doc.FirstChildElement("Actor");
	if (root == nullptr)
	{
		return false;
	}

	TiXmlElement* transformElem = root->FirstChildElement("Transform");
	if (transformElem != nullptr)
	{
		auto transform = this->AddComponent<TransformComponent>();
		transform->ReadXML(root);
	}

	TiXmlElement* movementElem = root->FirstChildElement("MovementComponent");
	if (movementElem != nullptr)
	{
		auto movement = this->AddComponent<MovementComponent>();
		movement->ReadXML(root);
	}

	TiXmlElement* boundboxElem = root->FirstChildElement("BoundingBoxComponent");
	if (boundboxElem != nullptr)
	{
		auto boundbox = this->AddComponent<BoundingBoxComponent>();
		boundbox->ReadXML(root);
	}

	TiXmlElement* cameraElem = root->FirstChildElement("CameraComponent");
	if (cameraElem != nullptr)
	{
		auto camera = this->AddComponent<Camera>();
		camera->ReadXML(root);
	}

	TiXmlElement* cameraArmElem = root->FirstChildElement("CameraArmComponent");
	if (cameraArmElem != nullptr)
	{
		auto cameraArm = this->AddComponent<CameraArmComponent>();
		cameraArm->ReadXML(root);
	}

	TiXmlElement* arrowElem = root->FirstChildElement("ArrowComponent");
	if (arrowElem != nullptr)
	{
		auto arrow = this->AddComponent<ArrowComponent>();
		arrow->ReadXML(root);
	}

	TiXmlElement* staticmeshElem = root->FirstChildElement("StaticMeshComponent");
	if (staticmeshElem != nullptr)
	{
		auto staticmesh = this->AddComponent<StaticMeshComponent>();
		staticmesh->ReadXML(root);
	}

	TiXmlElement* skeletalmeshElem = root->FirstChildElement("SkeletalMeshComponent");
	if (skeletalmeshElem != nullptr)
	{
		auto skeletalmesh = this->AddComponent<SkeletalMeshComponent>();
		skeletalmesh->ReadXML(root);
	}

	TiXmlElement* animElem = root->FirstChildElement("AnimationComponent");
	if (animElem != nullptr)
	{
		auto anim = this->AddComponent<AnimationComponent>();
		anim->ReadXML(root);
	}

	return true;
}
