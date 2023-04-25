#include "PointLight.h"

PointLight::PointLight()
{
	this->Name = L"PointLight";
	LightComp = this->AddComponent<PointLightComponent>();
}

PointLight::~PointLight()
{

}

bool PointLight::SaveXML(std::wstring filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//루트 노드 추가
	TiXmlElement* pRoot = new TiXmlElement("PointLight");
	doc.LinkEndChild(pRoot);

	if (!Transform->WriteXML(pRoot))
	{
		OutputDebugString(L"EditorECS::PointLight::SaveXML:: Failed Write Transform Component.");
		return false;
	}

	if (!LightComp->WriteXML(pRoot))
	{
		OutputDebugString(L"EditorECS::PointLight::SaveXML:: Failed Write Point Light Components.");
		return false;
	}

	std::string saveFile(filename.begin(), filename.end());
	return doc.SaveFile(saveFile.c_str());
}

bool PointLight::LoadXML(std::wstring filename)
{
	TiXmlDocument doc;
	std::string readfile(filename.begin(), filename.end());
	if (!doc.LoadFile(readfile.c_str()))
	{
		return false;
	}

	TiXmlElement* root = doc.FirstChildElement("PointLight");
	if (!LightComp->ReadXML(root))
	{
		OutputDebugString(L"EditorECS::PointLight::SaveXML:: Failed Read Point Light Components.");
		return false;
	}

	if (!Transform->ReadXML(root))
	{
		OutputDebugString(L"EditorECS::PointLight::SaveXML:: Failed Read Transform Component.");
		return false;
	}

	return true;
}

bool PointLight::Save(std::string filename)
{
	std::wstring file(filename.begin(), filename.end());
	return SaveXML(file);
}

bool PointLight::Load(std::string filename)
{
	std::wstring file(filename.begin(), filename.end());
	return LoadXML(file);
}