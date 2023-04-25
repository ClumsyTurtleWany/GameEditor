#include "XmlParser.h"

bool ReadVector2(TiXmlAttribute* attribute, Vector2& dest)
{
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector2::Attribute is nullptr.\n");
		return false;
	}

	dest.x = attribute->DoubleValue();
	attribute = attribute->Next();

	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector2::Next Attribute is nullptr.\n");
		return false;
	}

	dest.y = attribute->DoubleValue();
	attribute = attribute->Next();

	return true;
}

bool ReadVector3(TiXmlAttribute* attribute, Vector3& dest)
{
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector3::Attribute is nullptr.\n");
		return false;
	}

	dest.x = attribute->DoubleValue();
	attribute = attribute->Next();
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector3::Next Attribute is nullptr.\n");
		return false;
	}

	dest.y = attribute->DoubleValue();
	attribute = attribute->Next();
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector3::Next Attribute is nullptr.\n");
		return false;
	}

	dest.z = attribute->DoubleValue();
	attribute = attribute->Next();

	return true;
}

bool ReadVector4(TiXmlAttribute* attribute, Vector4& dest)
{
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector4::Attribute is nullptr.\n");
		return false;
	}

	dest.x = attribute->DoubleValue();
	attribute = attribute->Next();
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector4::Next Attribute is nullptr.\n");
		return false;
	}

	dest.y = attribute->DoubleValue();
	attribute = attribute->Next();
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector4::Next Attribute is nullptr.\n");
		return false;
	}

	dest.z = attribute->DoubleValue();
	attribute = attribute->Next();
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadVector4::Next Attribute is nullptr.\n");
		return false;
	}

	dest.w = attribute->DoubleValue();
	attribute = attribute->Next();

	return true;
}

bool ReadWString(TiXmlAttribute* attribute, std::wstring& dest)
{
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadWString::Attribute is nullptr.\n");
		return false;
	}

	std::string str = attribute->Value();
	attribute = attribute->Next();
	dest.assign(str.begin(), str.end());

	return true;
}

bool ReadString(TiXmlAttribute* attribute, std::string& dest)
{
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadString::Attribute is nullptr.\n");
		return false;
	}

	dest = attribute->Value();
	attribute = attribute->Next();

	return true;
}

bool ReadDouble(TiXmlAttribute* attribute, double& dest)
{
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadDouble::Attribute is nullptr.\n");
		return false;
	}

	dest = attribute->DoubleValue();
	attribute = attribute->Next();

	return true;
}

bool ReadDouble(TiXmlAttribute* attribute, float& dest)
{
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadDouble::Attribute is nullptr.\n");
		return false;
	}

	dest = static_cast<float>(attribute->DoubleValue());
	attribute = attribute->Next();

	return true;
}

bool ReadBoolean(TiXmlAttribute* attribute, bool& dest)
{
	if (attribute == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::ReadDouble::Attribute is nullptr.\n");
		return false;
	}

	std::string value = attribute->Value();
	if (value == "true")
	{
		dest = true;
	}
	else if (value == "false")
	{
		dest = false;
	}
	else
	{
		return false;
	}
	attribute = attribute->Next();

	return true;
}

bool ReadVector2(TiXmlElement* parent, std::string key, Vector2& dest)
{
	TiXmlElement* pElement = parent->FirstChildElement(key.c_str());
	if (pElement == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pElement->FirstAttribute();
	if (!ReadVector2(pAttribute, dest))
	{
		std::string debug = "BaseCore::ReadVector2::Failed Get " + key + ".\n";
		OutputDebugStringA(debug.c_str());
		return false;
	}
}

bool ReadVector3(TiXmlElement* parent, std::string key, Vector3& dest)
{
	TiXmlElement* pElement = parent->FirstChildElement(key.c_str());
	if (pElement == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pElement->FirstAttribute();
	if (!ReadVector3(pAttribute, dest))
	{
		std::string debug = "BaseCore::ReadVector3::Failed Get " + key + ".\n";
		OutputDebugStringA(debug.c_str());
		return false;
	}
}

bool ReadVector4(TiXmlElement* parent, std::string key, Vector4& dest)
{
	TiXmlElement* pElement = parent->FirstChildElement(key.c_str());
	if (pElement == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pElement->FirstAttribute();
	if (!ReadVector4(pAttribute, dest))
	{
		std::string debug = "BaseCore::ReadVector4::Failed Get " + key + ".\n";
		OutputDebugStringA(debug.c_str());
		return false;
	}
}

bool ReadWString(TiXmlElement* parent, std::string key, std::wstring& dest)
{
	TiXmlElement* pElement = parent->FirstChildElement(key.c_str());
	if (pElement == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pElement->FirstAttribute();
	if (!ReadWString(pAttribute, dest))
	{
		std::string debug = "BaseCore::ReadWString::Failed Get " + key + ".\n";
		OutputDebugStringA(debug.c_str());
		return false;
	}
}

bool ReadString(TiXmlElement* parent, std::string key, std::string& dest)
{
	TiXmlElement* pElement = parent->FirstChildElement(key.c_str());
	if (pElement == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pElement->FirstAttribute();
	if (!ReadString(pAttribute, dest))
	{
		std::string debug = "BaseCore::ReadString::Failed Get " + key + ".\n";
		OutputDebugStringA(debug.c_str());
		return false;
	}
}

bool ReadDouble(TiXmlElement* parent, std::string key, double& dest)
{
	TiXmlElement* pElement = parent->FirstChildElement(key.c_str());
	if (pElement == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pElement->FirstAttribute();
	if (!ReadDouble(pAttribute, dest))
	{
		std::string debug = "BaseCore::ReadDouble::Failed Get " + key + ".\n";
		OutputDebugStringA(debug.c_str());
		return false;
	}
}

bool ReadDouble(TiXmlElement* parent, std::string key, float& dest)
{
	TiXmlElement* pElement = parent->FirstChildElement(key.c_str());
	if (pElement == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pElement->FirstAttribute();
	if (!ReadDouble(pAttribute, dest))
	{
		std::string debug = "BaseCore::ReadDouble::Failed Get " + key + ".\n";
		OutputDebugStringA(debug.c_str());
		return false;
	}
}

bool ReadBoolean(TiXmlElement* parent, std::string key, bool& dest)
{
	TiXmlElement* pElement = parent->FirstChildElement(key.c_str());
	if (pElement == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pElement->FirstAttribute();
	if (!ReadBoolean(pAttribute, dest))
	{
		std::string debug = "BaseCore::ReadBoolean::Failed Get " + key + ".\n";
		OutputDebugStringA(debug.c_str());
		return false;
	}
}

bool WriteVector2(TiXmlElement* parent, const Vector2& src, std::string key)
{
	if (parent == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::WriteVector2::Element is nullptr.\n");
		return false;
	}

	TiXmlElement* pElement = new TiXmlElement(key.c_str());
	parent->LinkEndChild(pElement);
	pElement->SetDoubleAttribute("x", src.x);
	pElement->SetDoubleAttribute("y", src.y);

	return true;
}

bool WriteVector3(TiXmlElement* parent, const Vector3& src, std::string key)
{
	if (parent == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::WriteVector3::Element is nullptr.\n");
		return false;
	}

	TiXmlElement* pElement = new TiXmlElement(key.c_str());
	parent->LinkEndChild(pElement);
	pElement->SetDoubleAttribute("x", src.x);
	pElement->SetDoubleAttribute("y", src.y);
	pElement->SetDoubleAttribute("z", src.z);

	return true;
}

bool WriteVector4(TiXmlElement* parent, const Vector4& src, std::string key)
{
	if (parent == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::WriteVector4::Element is nullptr.\n");
		return false;
	}

	TiXmlElement* pElement = new TiXmlElement(key.c_str());
	parent->LinkEndChild(pElement);
	pElement->SetDoubleAttribute("x", src.x);
	pElement->SetDoubleAttribute("y", src.y);
	pElement->SetDoubleAttribute("z", src.z);
	pElement->SetDoubleAttribute("w", src.w);

	return true;
}

bool WriteDouble(TiXmlElement* parent, const double& src, std::string key)
{
	if (parent == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::WriteVector4::Element is nullptr.\n");
		return false;
	}

	TiXmlElement* pElement = new TiXmlElement(key.c_str());
	parent->LinkEndChild(pElement);
	pElement->SetDoubleAttribute("val", src);

	return true;
}

bool WriteWString(TiXmlElement* parent, const std::wstring& src, std::string key)
{
	if (parent == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::WritrWstring::Element is nullptr.\n");
		return false;
	}

	TiXmlElement* pElement = new TiXmlElement(key.c_str());
	parent->LinkEndChild(pElement);

	std::string str(src.begin(), src.end());
	pElement->SetAttribute("val", str.c_str());

	return true;
}

bool WriteString(TiXmlElement* parent, const std::string& src, std::string key)
{
	if (parent == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::WriteString::Element is nullptr.\n");
		return false;
	}

	TiXmlElement* pElement = new TiXmlElement(key.c_str());
	parent->LinkEndChild(pElement);

	pElement->SetAttribute("val", src.c_str());

	return true;
}

bool WriteBoolean(TiXmlElement* parent, const bool& src, std::string key)
{
	if (parent == nullptr)
	{
		OutputDebugString(L"BaseCore::XmlParser::WriteBoolean::Element is nullptr.\n");
		return false;
	}

	TiXmlElement* pElement = new TiXmlElement(key.c_str());
	parent->LinkEndChild(pElement);

	if (src == true)
	{
		pElement->SetAttribute("val", "true");
	}
	else
	{
		pElement->SetAttribute("val", "false");
	}
	
	return true;
}
