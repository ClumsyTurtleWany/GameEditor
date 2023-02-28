#pragma once
#include "Define.h"
#include "DXUserInterface.h"

enum class ButtonState
{
	Normal,
	Hover,
	Pressed,
	Disable
};

class DXButton : public DXUserInterface
{
private:
	DXTexture* NormalImage = nullptr;
	DXTexture* HoverImage = nullptr;
	DXTexture* PressedImage = nullptr;
	DXTexture* DisableImage = nullptr;
	DXTexture* RenderImage = nullptr;

	std::vector<Vertex> Vertices;
	std::vector<DWORD> Indecies;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;

	ID3D11PixelShader* TexturedPS = nullptr;
	ID3D11PixelShader* NonTexturedPS = nullptr;

private:
	Vector3 Position = Vector3(0.0f, 0.0f, 0.0f);
	float Width = 0.0f;
	float Height = 0.0f;

	bool Enable = true;
	bool IsCreated = false;
	ButtonState CurrentState = ButtonState::Normal;
	ButtonState BeforeState = ButtonState::Normal;

	Vector4 NormalColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 HoverColor = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
	Vector4 PressedColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 DisableColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

private:
	std::function<void()> CallbackFunction;

public:
	DXButton();
	virtual ~DXButton();

protected:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;

public:
	bool Create(Vector3 pos, float width, float height, 
		DXTexture* normalImg = nullptr, DXTexture* hoverImg = nullptr, DXTexture* pressedImg = nullptr, DXTexture* disableImg = nullptr,
		Vector4 normalColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4 hoverColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4 pressedColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4 disableColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f));

private:
	void SetVertexColor(Vector4 color);
};