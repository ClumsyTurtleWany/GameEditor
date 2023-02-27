#include "pch.h"
#include "DaeFile.h"

bool DaeFile::Load(std::string filename)
{
	TiXmlDocument doc;
	if (!doc.LoadFile(filename.c_str()))
	{
		return false;
	}

	// Root Node
	TiXmlElement* root = doc.FirstChildElement();

	// Asset Information
	TiXmlElement* asset = root->FirstChildElement("asset");
	if (!ParseAssetInfo(asset))
	{
		OutputDebugString(L"Asset info is null");
	}
	

	// Library images == Texture images
	TiXmlElement* images = root->FirstChildElement("library_images");
	/*TiXmlElement* image = images->FirstChildElement("image");
	for (TiXmlElement* target = image; target != nullptr; target = target->NextSiblingElement())
	{
		TiXmlAttribute* attribute = target->FirstAttribute();
		std::string name = attribute->Name();
		std::string id = attribute->Value();

		TiXmlElement* ele = target->FirstChildElement("init_from")->FirstChildElement("ref");
		std::string textureName = ele->GetText();

		textureImages.insert(std::make_pair(id, textureName));
	}*/
	if (!ParseImages(images))
	{
		OutputDebugString(L"Texture images info is null");
	}

	// Library effects
	TiXmlElement* effects = root->FirstChildElement("library_effects");
	/*TiXmlElement* effect = effects->FirstChildElement("effect");
	for (TiXmlElement* target = effect; target != nullptr; target = target->NextSiblingElement())
	{
		TiXmlAttribute* attribute = target->FirstAttribute();
		std::string name = attribute->Name();
		std::string id = attribute->Value();

		TiXmlElement* profile = target->FirstChildElement("profile_COMMON");
		TiXmlElement* technique = profile->FirstChildElement("technique");
		if (technique != nullptr)
		{
			attribute = technique->FirstAttribute();
			std::string id = attribute->Value();

			TiXmlElement* blinn = technique->FirstChildElement("blinn");
			TiXmlElement* diffuse = blinn->FirstChildElement("diffuse")->FirstChildElement("color");
			if (diffuse != nullptr)
			{
				std::string val = diffuse->GetText();
			}
		}
		
		TiXmlElement* newparam = profile->FirstChildElement("newparam");
		if (newparam != nullptr)
		{

		}
	}*/
	if (!ParseEffects(effects))
	{
		OutputDebugString(L"Texture images info is null");
	}

	// Library geometries
	TiXmlElement* geometries = root->FirstChildElement("library_geometries");
	/*TiXmlElement* geometry = geometries->FirstChildElement("geometry");
	for (TiXmlElement* target = geometry; target != nullptr; target = target->NextSiblingElement())
	{
		TiXmlAttribute* attribute = target->FirstAttribute();
		std::string name = attribute->Name();
		std::string id = attribute->Value();

		TiXmlElement* source = target->FirstChildElement("mesh")->FirstChildElement("source");
		for (TiXmlElement* ts = source; ts != nullptr; ts = ts->NextSiblingElement())
		{
			attribute = ts->FirstAttribute();
			std::string sid = attribute->Value();

			TiXmlElement* float_array = ts->FirstChildElement("float_array");
			attribute = float_array->FirstAttribute();
			std::string array_id = attribute->Value();
			attribute = attribute->Next();
			std::string count = attribute->Value();
			attribute = attribute->Next();
			std::string digits = attribute->Value();

			std::string arrayData = float_array->GetText();

			std::vector<std::string> rst;
			if (SplitString(arrayData, ' ', rst))
			{

			}
			int a = 0;

		}
	}*/
	if (!ParseGeometries(geometries))
	{
		OutputDebugString(L"Texture images info is null");
	}

	return false;
}

bool DaeFile::SplitString(std::string line, char delimiter, std::vector<std::string>& dst)
{
	std::istringstream ss(line);
	std::string word;
	bool isValid = false;
	while (std::getline(ss, word, delimiter))
	{
		isValid = true;
		dst.push_back(word);
	}

	return isValid;
}

bool DaeFile::ParseAssetInfo(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* upAxis = parent->FirstChildElement("up_axis");
	if (upAxis != nullptr)
	{
		_axis[2] = upAxis->GetText();
	}

	return true;
}

bool DaeFile::ParseImages(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* image = parent->FirstChildElement("image");
	for (TiXmlElement* target = image; target != nullptr; target = target->NextSiblingElement())
	{
		TiXmlAttribute* attribute = target->FirstAttribute();
		if (attribute == nullptr)
		{
			continue;
		}
		
		std::string name = attribute->Name();
		std::string id = attribute->Value();
		
		TiXmlElement* ele = target->FirstChildElement("init_from")->FirstChildElement("ref");
		if (ele == nullptr)
		{
			continue;
		}
		std::string textureName = ele->GetText();

		_textureImages.insert(std::make_pair(id, textureName));
	}

	return true;
}

bool DaeFile::ParseEffects(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* effect = parent->FirstChildElement("effect");
	for (TiXmlElement* target = effect; target != nullptr; target = target->NextSiblingElement())
	{
		TiXmlAttribute* attribute = target->FirstAttribute();
		if (attribute == nullptr)
		{
			continue;
		}

		std::string name = attribute->Name();
		std::string id = attribute->Value();

		TiXmlElement* profile = target->FirstChildElement("profile_COMMON");
		if (profile == nullptr)
		{
			continue;
		}

		TiXmlElement* technique = profile->FirstChildElement("technique");
		if (technique == nullptr)
		{
			continue;
		}

		attribute = technique->FirstAttribute();
		if (attribute == nullptr)
		{
			continue;
		}

		std::string id2 = attribute->Value();

		TiXmlElement* blinn = technique->FirstChildElement("blinn");
		if (blinn == nullptr)
		{
			continue;
		}

		TiXmlElement* diffuse = blinn->FirstChildElement("diffuse")->FirstChildElement("color");
		if (diffuse == nullptr)
		{
			continue;
		}
		
		std::string diffuseColor = diffuse->GetText();

		TiXmlElement* newparam = profile->FirstChildElement("newparam");
		if (newparam != nullptr)
		{

		}
	}

	return true;
}

bool DaeFile::ParseMaterials(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	return true;
}

bool DaeFile::ParseGeometries(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	// [0]library_geometries		
	// - [1]geometry				: attribute - id
	// -- [2]mesh
	// --- [3]source				: attribute - id
	// ---- [4]float_array			: attribute - id(string), count(int), digits(int) == length
	// ---- [4]technique_common	:
	// ----- [5]accessor			: attribute - vertices count(int), 적용할 소스 id(string), stride(int) 
	// ------ [6]param				: attribute - name(string, x, y, z 혹은 u,v 순서), type(string, ex)"float")
	// --- [3]vertices				: attribute - id(string)
	// ---- [4]input				: attribute - semantic(string, "POSITION"), source(string, 적용할 소스 id)
	// --- [3]triangles				: attribute - count(int), material(string, 머테리얼)
	// ---- [4]input				: attribute - offset(int), semantic(string, "VERTEX, NORMAL, TEXCOORD"), source(string, 적용할 소스 ID), set(int)

	for (TiXmlElement* geometry = parent->FirstChildElement("geometry"); geometry != nullptr; geometry = geometry->NextSiblingElement())
	{
		TiXmlAttribute* attribute = geometry->FirstAttribute();
		if (attribute == nullptr)
		{
			continue;
		}

		std::string name = attribute->Name();
		std::string id = attribute->Value();

		TiXmlElement* mesh = geometry->FirstChildElement("mesh");
		if (!ParseGeometryMesh(mesh, id))
		{
			continue;
		}

		/*TiXmlElement* source = geometry->FirstChildElement("mesh")->FirstChildElement("source");
		for (TiXmlElement* ts = source; ts != nullptr; ts = ts->NextSiblingElement())
		{
			attribute = ts->FirstAttribute();
			if (attribute == nullptr)
			{
				continue;
			}
			std::string sid = attribute->Value();

			TiXmlElement* float_array = ts->FirstChildElement("float_array");
			if (float_array == nullptr)
			{
				continue;
			}

			attribute = float_array->FirstAttribute();
			if (attribute == nullptr)
			{
				continue;
			}
			std::string array_id = attribute->Value();

			attribute = attribute->Next();
			std::string count = attribute->Value();
			attribute = attribute->Next();
			std::string digits = attribute->Value();

			std::string arrayData = float_array->GetText();

			std::vector<std::string> rst;
			if (SplitString(arrayData, ' ', rst))
			{

			}
			int a = 0;

		}*/
	}

	return true;
}

bool DaeFile::ParseGeometryMesh(TiXmlElement* parent, std::string id)
{
	if (parent == nullptr)
	{
		return false;
	}

	// [0]library_geometries		
	// - [1]geometry				: attribute - id
	// -- [2]mesh
	// --- [3]source				: attribute - id
	// ---- [4]float_array			: attribute - id(string), count(int), digits(int) == length
	// ---- [4]technique_common	:
	// ----- [5]accessor			: attribute - vertices count(int), 적용할 소스 id(string), stride(int) 
	// ------ [6]param				: attribute - name(string, x, y, z 혹은 u,v 순서), type(string, ex)"float")
	// --- [3]vertices				: attribute - id(string)
	// ---- [4]input				: attribute - semantic(string, "POSITION"), source(string, 적용할 소스 id)
	// --- [3]triangles				: attribute - count(int), material(string, 머테리얼)
	// ---- [4]input				: attribute - offset(int), semantic(string, "VERTEX, NORMAL, TEXCOORD"), source(string, 적용할 소스 ID), set(int)

	for (TiXmlElement* source = parent->FirstChildElement("source"); source != nullptr; source = source->NextSiblingElement())
	{
		if (!ParseGeometryMeshSource(source))
		{
			continue;
		}
	}

	for (TiXmlElement* vertices = parent->FirstChildElement("vertices"); vertices != nullptr; vertices = vertices->NextSiblingElement())
	{
		if (!ParseGeometryMeshVertices(vertices))
		{
			continue;
		}
	}

	for (TiXmlElement* triangles = parent->FirstChildElement("triangles"); triangles != nullptr; triangles = triangles->NextSiblingElement())
	{
		if (!ParseGeometryMeshTriangles(triangles))
		{
			continue;
		}
	}
	return true;
}

bool DaeFile::ParseGeometryMeshSource(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	// [0]library_geometries		
	// - [1]geometry				: attribute - id
	// -- [2]mesh
	// --- [3]source				: attribute - id
	// ---- [4]float_array			: attribute - id(string), count(int), digits(int) == length
	// ---- [4]technique_common
	// ----- [5]accessor			: attribute - vertices count(int), 적용할 소스 id(string), stride(int) 
	// ------ [6]param				: attribute - name(string, x, y, z 혹은 u,v 순서), type(string, ex)"float")
	// --- [3]vertices				: attribute - id(string)
	// ---- [4]input				: attribute - semantic(string, "POSITION"), source(string, 적용할 소스 id)
	// --- [3]triangles				: attribute - count(int), material(string, 머테리얼)
	// ---- [4]input				: attribute - offset(int), semantic(string, "VERTEX, NORMAL, TEXCOORD"), source(string, 적용할 소스 ID), set(int)

	TiXmlAttribute* attribute = parent->FirstAttribute();
	if (attribute == nullptr)
	{
		return false;
	}
	std::string sid = attribute->Value();

	//--------------------------------------
	// Parse float_array
	//--------------------------------------
	TiXmlElement* float_array = parent->FirstChildElement("float_array");
	attribute = float_array->FirstAttribute();
	if (float_array == nullptr || attribute == nullptr)
	{
		return false;
	}

	std::string array_id = attribute->Value(); attribute = attribute->Next();
	std::string count = attribute->Value(); attribute = attribute->Next();
	std::string digits = attribute->Value();
	std::string arrayData = float_array->GetText();

	std::vector<std::string> rst;
	if (SplitString(arrayData, ' ', rst))
	{

	}

	//--------------------------------------
	// Parse accessor
	//--------------------------------------
	TiXmlElement* accessor = parent->FirstChildElement("technique_common")->FirstChildElement("accessor");
	attribute = accessor->FirstAttribute();
	if (accessor == nullptr || attribute == nullptr)
	{
		return false;
	}

	int verticesCount = std::stoi(attribute->Value()); attribute = attribute->Next();
	std::string accessorSource = attribute->Value(); attribute = attribute->Next();
	int stride = std::stoi(attribute->Value());

	std::vector<std::string> paramList;
	for (TiXmlElement* param = accessor->FirstChildElement("param"); param != nullptr; param = param->NextSiblingElement())
	{
		attribute = param->FirstAttribute();
		if (param == nullptr || attribute == nullptr)
		{
			continue;
		}

		std::string axis = attribute->Value();
		paramList.push_back(axis);
	}

	return true;
}

bool DaeFile::ParseGeometryMeshVertices(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	// [0]library_geometries		
	// - [1]geometry				: attribute - id
	// -- [2]mesh
	// --- [3]source				: attribute - id
	// ---- [4]float_array			: attribute - id(string), count(int), digits(int) == length
	// ---- [4]technique_common	:
	// ----- [5]accessor			: attribute - vertices count(int), 적용할 소스 id(string), stride(int) 
	// ------ [6]param				: attribute - name(string, x, y, z 혹은 u,v 순서), type(string, ex)"float")
	// --- [3]vertices				: attribute - id(string)
	// ---- [4]input				: attribute - semantic(string, "POSITION"), source(string, 적용할 소스 id)
	// --- [3]triangles				: attribute - count(int), material(string, 머테리얼)
	// ---- [4]input				: attribute - offset(int), semantic(string, "VERTEX, NORMAL, TEXCOORD"), source(string, 적용할 소스 ID), set(int)

	TiXmlAttribute* attribute = parent->FirstAttribute();
	if (attribute == nullptr)
	{
		return false;
	}

	std::string id = attribute->Value();

	TiXmlElement* input = parent->FirstChildElement("input");
	attribute = input->FirstAttribute();
	if (input == nullptr || attribute == nullptr)
	{
		return false;
	}

	std::string semantic = attribute->Value(); attribute = attribute->Next();
	std::string source = attribute->Value();

	return true;
}

bool DaeFile::ParseGeometryMeshTriangles(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	// [0]library_geometries		
	// - [1]geometry				: attribute - id
	// -- [2]mesh
	// --- [3]source				: attribute - id
	// ---- [4]float_array			: attribute - id(string), count(int), digits(int) == length
	// ---- [4]technique_common	:
	// ----- [5]accessor			: attribute - vertices count(int), 적용할 소스 id(string), stride(int) 
	// ------ [6]param				: attribute - name(string, x, y, z 혹은 u,v 순서), type(string, ex)"float")
	// --- [3]vertices				: attribute - id(string)
	// ---- [4]input				: attribute - semantic(string, "POSITION"), source(string, 적용할 소스 id)
	// --- [3]triangles				: attribute - count(int), material(string, 머테리얼)
	// ---- [4]input				: attribute - offset(int), semantic(string, "VERTEX, NORMAL, TEXCOORD"), source(string, 적용할 소스 ID), set(int)

	TiXmlAttribute* attribute = parent->FirstAttribute();
	if (attribute == nullptr)
	{
		return false;
	}

	int count = std::stoi(attribute->Value()); attribute = attribute->Next();
	std::string material = attribute->Value();

	for (TiXmlElement* input = parent->FirstChildElement("input"); input != nullptr; input = input->NextSiblingElement())
	{
		if (input == nullptr)
		{
			continue;
		}

		attribute = input->FirstAttribute();
		if (attribute == nullptr)
		{
			continue;
		}

		int offset = std::stoi(attribute->Value()); attribute = attribute->Next();
		std::string semantic = attribute->Value(); attribute = attribute->Next();
		std::string source = attribute->Value(); attribute = attribute->Next();
		int set = std::stoi(attribute->Value());
	}

	TiXmlElement* p = parent->FirstChildElement("p");
	if (p == nullptr)
	{
		return false;
	}

	std::string indexString = p->GetText();
	std::vector<std::string> indexStringList;
	if (!SplitString(indexString, ' ', indexStringList))
	{

	}

	return true;
}
