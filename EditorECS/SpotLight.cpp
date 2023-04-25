#include "SpotLight.h"

SpotLight::SpotLight()
{
	this->Name = L"SpotLight";
	LightComp = this->AddComponent<SpotLightComponent>();
}

SpotLight::~SpotLight()
{

}

bool SpotLight::SaveXML(std::wstring filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//루트 노드 추가
	TiXmlElement* pRoot = new TiXmlElement("SpotLight");
	doc.LinkEndChild(pRoot);

	if (!Transform->WriteXML(pRoot))
	{
		OutputDebugString(L"EditorECS::SpotLight::SaveXML:: Failed Write Transform Component.");
		return false;
	}

	if (!LightComp->WriteXML(pRoot))
	{
		OutputDebugString(L"EditorECS::SpotLight::SaveXML:: Failed Write Point Light Components.");
		return false;
	}

	std::string saveFile(filename.begin(), filename.end());
	return doc.SaveFile(saveFile.c_str());
}

bool SpotLight::LoadXML(std::wstring filename)
{
	TiXmlDocument doc;
	std::string readfile(filename.begin(), filename.end());
	if (!doc.LoadFile(readfile.c_str()))
	{
		return false;
	}

	TiXmlElement* root = doc.FirstChildElement("SpotLight");
	if (!LightComp->ReadXML(root))
	{
		OutputDebugString(L"EditorECS::SpotLight::SaveXML:: Failed Read Spot Light Components.");
		return false;
	}

	if (!Transform->ReadXML(root))
	{
		OutputDebugString(L"EditorECS::SpotLight::SaveXML:: Failed Read Transform Component.");
		return false;
	}

	return true;
}

bool SpotLight::Save(std::string filename)
{
	std::wstring file(filename.begin(), filename.end());
	return SaveXML(file);
}

bool SpotLight::Load(std::string filename)
{
	std::wstring file(filename.begin(), filename.end());
	return LoadXML(file);
}