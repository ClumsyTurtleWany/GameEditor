#pragma once
#include "DXTK/SimpleMath.h"
#include "tinystr.h"
#include "tinyxml.h"

using namespace DirectX::SimpleMath;

bool ReadVector2(TiXmlAttribute* attribute, Vector2& dest);
bool ReadVector3(TiXmlAttribute* attribute, Vector3& dest);
bool ReadVector4(TiXmlAttribute* attribute, Vector4& dest);
bool ReadWString(TiXmlAttribute* attribute, std::wstring& dest);
bool ReadString(TiXmlAttribute* attribute, std::string& dest);
bool ReadDouble(TiXmlAttribute* attribute, double& dest);
bool ReadDouble(TiXmlAttribute* attribute, float& dest);
bool ReadBoolean(TiXmlAttribute* attribute, bool& dest);

bool ReadVector2(TiXmlElement* parent, std::string key, Vector2& dest);
bool ReadVector3(TiXmlElement* parent, std::string key, Vector3& dest);
bool ReadVector4(TiXmlElement* parent, std::string key, Vector4& dest);
bool ReadWString(TiXmlElement* parent, std::string key, std::wstring& dest);
bool ReadString(TiXmlElement* parent, std::string key, std::string& dest);
bool ReadDouble(TiXmlElement* parent, std::string key, double& dest);
bool ReadDouble(TiXmlElement* parent, std::string key, float& dest);
bool ReadBoolean(TiXmlElement* parent, std::string key, bool& dest);

	
bool WriteVector2(TiXmlElement* parent, const Vector2& src, std::string key);
bool WriteVector3(TiXmlElement* parent, const Vector3& src, std::string key);
bool WriteVector4(TiXmlElement* parent, const Vector4& src, std::string key);
bool WriteDouble(TiXmlElement* parent, const double& src, std::string key);

bool WriteWString(TiXmlElement* parent, const std::wstring& src, std::string key);
bool WriteString(TiXmlElement* parent, const std::string& src, std::string key);
bool WriteBoolean(TiXmlElement* parent, const bool& src, std::string key);
