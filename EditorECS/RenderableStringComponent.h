#pragma once
#include "EffectInclude\EffectDefine.h"

class RenderableStringComponent
{
public:
	std::wstring wszText;

public:
	RenderableStringComponent();
	RenderableStringComponent(std::wstring str);
	virtual ~RenderableStringComponent();
};