#include "Landscape.h"
#include "TransformComponent.h"
#include "DXSamplerState.hpp"

Landscape::Landscape()
{
	this->Name = L"Landscape"; // +std::to_wstring(this->ID);
	Transform = this->AddComponent<TransformComponent>();
	Components = this->AddComponent<LandscapeComponents>();
	Components->Transform = Transform;
	Components->Initialize();
}

Landscape::~Landscape()
{

}

bool Landscape::SaveXML(std::wstring filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//루트 노드 추가
	TiXmlElement* pRoot = new TiXmlElement("Landscape");
	doc.LinkEndChild(pRoot);

	if (!Transform->WriteXML(pRoot))
	{
		OutputDebugString(L"EditorECS::Landscape::SaveXML:: Failed Write Transform Component.");
		return false;
	}

	if (!Components->WriteXML(pRoot))
	{
		OutputDebugString(L"EditorECS::Landscape::SaveXML:: Failed Write Landscape Components.");
		return false;
	}

	std::string saveFile(filename.begin(), filename.end());
	return doc.SaveFile(saveFile.c_str());
}

bool Landscape::LoadXML(std::wstring filename)
{
	TiXmlDocument doc;
	std::string readfile(filename.begin(), filename.end());
	if (!doc.LoadFile(readfile.c_str()))
	{
		return false;
	}

	TiXmlElement* root = doc.FirstChildElement("Landscape");
	if (!Components->ReadXML(root))
	{
		OutputDebugString(L"EditorECS::Landscape::SaveXML:: Failed Read Landscape Components.");
		return false;
	}

	if (!Transform->ReadXML(root))
	{
		OutputDebugString(L"EditorECS::Landscape::SaveXML:: Failed Read Transform Component.");
		return false;
	}

	return true;
}

bool Landscape::Save(std::string filename)
{
	std::wstring file(filename.begin(), filename.end());
	return SaveXML(file);
}

bool Landscape::Load(std::string filename)
{
	std::wstring file(filename.begin(), filename.end());
	return LoadXML(file);
}
