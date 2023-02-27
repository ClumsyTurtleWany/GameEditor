#pragma once
#include <fstream>
#include <sstream>

#include "Define.h"
#include "tinystr.h"
#include "tinyxml.h"

class DaeFile
{
public:
	// Asset Infomation
	// _axis[0]: front
	// _axis[1]: right
	// _axis[2]: up
	std::string _axis[3];

	// Texture Images name
	std::map<std::string, std::string> _textureImages;

public:
	//----------------------------------------
	// Common Function
	//----------------------------------------
	bool Load(std::string filename);
	bool SplitString(std::string line, char delimiter, std::vector<std::string>& dst);

public:
	//----------------------------------------
	// Parser Function
	//----------------------------------------

	// Asset Infomation
	bool ParseAssetInfo(TiXmlElement* parent);

	// Texture images name
	bool ParseImages(TiXmlElement* parent);

	// Texture process method
	bool ParseEffects(TiXmlElement* parent);

	// Material
	bool ParseMaterials(TiXmlElement* parent);

	// Geometries -> Geometry
	bool ParseGeometries(TiXmlElement* parent);
	bool ParseGeometryMesh(TiXmlElement* parent, std::string id);
	bool ParseGeometryMeshSource(TiXmlElement* parent);
	bool ParseGeometryMeshVertices(TiXmlElement* parent);
	bool ParseGeometryMeshTriangles(TiXmlElement* parent);
};