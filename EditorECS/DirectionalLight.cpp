#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	this->Name = L"DirectionalLight"; // +std::to_wstring(this->ID);
	LightComp = this->AddComponent<DirectionalLightComponent>();
}

DirectionalLight::~DirectionalLight()
{

}

bool DirectionalLight::SaveXML(std::wstring filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//루트 노드 추가
	TiXmlElement* pRoot = new TiXmlElement("DirectionalLight");
	doc.LinkEndChild(pRoot);

	if (!Transform->WriteXML(pRoot))
	{
		OutputDebugString(L"EditorECS::DirectionalLight::SaveXML:: Failed Write Transform Component.");
		return false;
	}

	if (!LightComp->WriteXML(pRoot))
	{
		OutputDebugString(L"EditorECS::DirectionalLight::SaveXML:: Failed Write Directional Light Components.");
		return false;
	}

	std::string saveFile(filename.begin(), filename.end());
	return doc.SaveFile(saveFile.c_str());
}

bool DirectionalLight::LoadXML(std::wstring filename)
{
	TiXmlDocument doc;
	std::string readfile(filename.begin(), filename.end());
	if (!doc.LoadFile(readfile.c_str()))
	{
		return false;
	}

	TiXmlElement* root = doc.FirstChildElement("DirectionalLight");
	if (!LightComp->ReadXML(root))
	{
		OutputDebugString(L"EditorECS::DirectionalLight::SaveXML:: Failed Read Directional Light Components.");
		return false;
	}

	if (!Transform->ReadXML(root))
	{
		OutputDebugString(L"EditorECS::DirectionalLight::SaveXML:: Failed Read Transform Component.");
		return false;
	}

	return true;
}

bool DirectionalLight::Save(std::string filename)
{
	std::wstring file(filename.begin(), filename.end());
	return SaveXML(file);
}

bool DirectionalLight::Load(std::string filename)
{
	std::wstring file(filename.begin(), filename.end());
	return LoadXML(file);
}
