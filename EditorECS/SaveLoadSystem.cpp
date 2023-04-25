#include "SaveLoadSystem.h"
#include <filesystem>
#include "Actor.h"
#include "Landscape.h"
#include "Character.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"

void SaveLoadSystem::Tick(ECS::World* world, float time)
{
	if (SaveFlag)
	{
		TiXmlDocument doc;
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
		doc.LinkEndChild(decl);

		TiXmlElement* pRoot = new TiXmlElement("World");
		doc.LinkEndChild(pRoot);

		std::string path = Path + "/" + SaveFileName + "/";
		if (!std::filesystem::exists(path))
		{
			std::filesystem::create_directory(path);
		}
		
		for (auto& it : world->GetAllEntities())
		{
			std::wstring wstrName = it.get()->GetName();
			std::string id = std::to_string(it.get()->GetID());
			std::string type(wstrName.begin(), wstrName.end());
			if (type.empty())
			{
				continue;
			}

			TiXmlElement* pElement = new TiXmlElement(type.c_str());
			pRoot->LinkEndChild(pElement);

			//pElement->SetAttribute("Type", type.c_str());
			//pElement->SetAttribute("ID", id.c_str());
			std::string filename = path + type + id + ".entity";
			//pElement->SetAttribute("Path", filename.c_str());
			if (!WriteString(pElement, id, "ID"))
			{

			}

			if (!WriteString(pElement, filename, "Path"))
			{

			}

			it.get()->Save(filename);
		}

		std::string worldFile = Path + "/" + SaveFileName + ".world";
		doc.SaveFile(worldFile.c_str());

		SaveFlag = false;
	}

	if (LoadFlag)
	{
		TiXmlDocument doc;
		std::string readfile = Path + "/" + LoadFileName + ".world";
		if (doc.LoadFile(readfile.c_str()))
		{
			TiXmlElement* root = doc.FirstChildElement("World");
			TiXmlElement* actor = root->FirstChildElement("Actor");
			while (actor != nullptr)
			{
				std::string path;
				if (ReadString(actor, "Path", path))
				{
					Actor* newEntity = new Actor;
					if (newEntity->Load(path))
					{
						world->AddEntity(newEntity);
					}
					else
					{
						delete newEntity;
					}
				}
				actor = actor->NextSiblingElement();
			}

			TiXmlElement* landscape = root->FirstChildElement("Landscape");
			while (landscape != nullptr)
			{
				std::string path;
				if (ReadString(landscape, "Path", path))
				{
					Landscape* newEntity = new Landscape;
					if (newEntity->Load(path))
					{
						world->AddEntity(newEntity);
					}
					else
					{
						delete newEntity;
					}
				}
				landscape = landscape->NextSiblingElement();
			}

			bool bLoadCharacter = false;
			if (bLoadCharacter)
			{
				TiXmlElement* character = root->FirstChildElement("Character");
				while (character != nullptr)
				{
					std::string path;
					if (ReadString(character, "Path", path))
					{
						Character* newEntity = new Character;
						if (newEntity->Load(path))
						{
							world->AddEntity(newEntity);
						}
						else
						{
							delete newEntity;
						}
					}
					character = character->NextSiblingElement();
				}
			}

			TiXmlElement* directionalLight = root->FirstChildElement("DirectionalLight");
			while (directionalLight != nullptr)
			{
				std::string path;
				if (ReadString(directionalLight, "Path", path))
				{
					DirectionalLight* newEntity = new DirectionalLight;
					if (newEntity->Load(path))
					{
						world->AddEntity(newEntity);
					}
					else
					{
						delete newEntity;
					}
				}
				directionalLight = directionalLight->NextSiblingElement();
			}

			TiXmlElement* spotLight = root->FirstChildElement("SpotLight");
			while (spotLight != nullptr)
			{
				std::string path;
				if (ReadString(spotLight, "Path", path))
				{
					SpotLight* newEntity = new SpotLight;
					if (newEntity->Load(path))
					{
						world->AddEntity(newEntity);
					}
					else
					{
						delete newEntity;
					}
				}
				spotLight = spotLight->NextSiblingElement();
			}

			TiXmlElement* pointLight = root->FirstChildElement("PointLight");
			while (pointLight != nullptr)
			{
				std::string path;
				if (ReadString(pointLight, "Path", path))
				{
					PointLight* newEntity = new PointLight;
					if (newEntity->Load(path))
					{
						world->AddEntity(newEntity);
					}
					else
					{
						delete newEntity;
					}
				}
				pointLight = pointLight->NextSiblingElement();
			}
		}
		LoadFlag = false;
	}
}

void SaveLoadSystem::Save(std::string path, std::string filename)
{
	SaveFlag = true;
	Path = path;
	SaveFileName = filename;
}

void SaveLoadSystem::Load(std::string path, std::string filename)
{
	LoadFlag = true;
	Path = path;
	LoadFileName = filename;
}
