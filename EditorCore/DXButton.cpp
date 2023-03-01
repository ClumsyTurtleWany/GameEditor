#include "DXButton.h"
#include "DXUserInterfaceManager.h"

DXButton::DXButton()
{
}

DXButton::~DXButton()
{
}

bool DXButton::Initialize()
{
    Vertices.resize(4);
    Vertices[0].Pos = Position;
    Vertices[1].Pos = Vector3(Position.x + Width, Position.y, Position.z);
    Vertices[2].Pos = Vector3(Position.x, Position.y + Height, Position.z);
    Vertices[3].Pos = Vector3(Position.x + Width, Position.y + Height, Position.z);

    Vertices[0].Color = NormalColor;
    Vertices[1].Color = NormalColor;
    Vertices[2].Color = NormalColor;
    Vertices[3].Color = NormalColor;

    Vertices[0].Texture = Vector2(0.0f, 0.0f);
    Vertices[1].Texture = Vector2(1.0f, 0.0f);
    Vertices[2].Texture = Vector2(0.0f, 1.0f);
    Vertices[3].Texture = Vector2(1.0f, 1.0f);

    Indecies.resize(6);
    Indecies[0] = 0;
    Indecies[1] = 1;
    Indecies[2] = 2;
    Indecies[3] = 2;
    Indecies[4] = 1;
    Indecies[5] = 3;

    VertexBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer<Vertex>(Vertices);
    IndexBuffer = DXShaderManager::GetInstance()->CreateIndexBuffer(Indecies);

    TexturedPS = DXShaderManager::GetInstance()->GetPixelShader(L"../include/HLSL/PS_TexturedUI.hlsl");
    NonTexturedPS = DXShaderManager::GetInstance()->GetPixelShader(L"../include/HLSL/PS_NonTexturedUI.hlsl");

    this->Context = DXUserInterfaceManager::GetInstance()->GetContext();
    DXUserInterfaceManager::GetInstance()->AddUserInterface(this);

    return true;
}

bool DXButton::Frame()
{
    POINT Cursor = Input::GetInstance()->m_ptPos;
    if (Cursor.x < Position.x)
    {
        CurrentState = ButtonState::Normal;
    }
    else if (Cursor.x > Position.x + Width)
    {
        CurrentState = ButtonState::Normal;
    }
    else if (Cursor.y < Position.y)
    {
        CurrentState = ButtonState::Normal;
    }
    else if (Cursor.y > Position.y + Height)
    {
        CurrentState = ButtonState::Normal;
    }
    else
    {
        CurrentState = ButtonState::Hover;
        KeyState leftBtn = Input::GetInstance()->getKey(VK_LBUTTON);
        if (leftBtn == KeyState::Down || leftBtn == KeyState::Hold)
        {
            CurrentState = ButtonState::Pressed;
        }
        else
        {
            if (BeforeState == ButtonState::Pressed)
            {
                CallbackFunction();
            }
        }
    }

    if (!Enable)
    {
        CurrentState = ButtonState::Disable;
    }

    BeforeState = CurrentState;

    return true;
}

bool DXButton::Render()
{
    Context->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);
    
    UINT strides = sizeof(Vertex);
    UINT offsets = 0;
    Context->IAGetVertexBuffers(0, 1, &VertexBuffer, &strides, &offsets);

    if (CurrentState == ButtonState::Normal)
    {
        RenderImage = NormalImage;
        SetVertexColor(NormalColor);
    }
    else if (CurrentState == ButtonState::Hover)
    {
        RenderImage = HoverImage;
        SetVertexColor(HoverColor);
    }
    else if (CurrentState == ButtonState::Pressed)
    {
        RenderImage = PressedImage;
        SetVertexColor(PressedColor);
    }
    else if(CurrentState == ButtonState::Disable)
    {
        RenderImage = DisableImage;
        SetVertexColor(DisableColor);
    }
    else
    {
        RenderImage = nullptr;
    }

    if (RenderImage != nullptr)
    {
        ID3D11ShaderResourceView* resourceView = RenderImage->getResourceView();
        Context->PSSetShaderResources(0, 1, &resourceView);

        Context->PSSetShader(TexturedPS, NULL, 0);
    }
    else
    {
        Context->PSSetShader(NonTexturedPS, NULL, 0);
    }

    Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Context->DrawIndexed(static_cast<UINT>(Indecies.size()), 0, 0);

    return true;
}

bool DXButton::Release()
{
    /*if (VertexBuffer != nullptr)
    {
        VertexBuffer->Release();
        VertexBuffer = nullptr;
    }

    if (IndexBuffer != nullptr)
    {
        IndexBuffer->Release();
        IndexBuffer = nullptr;
    }*/

    return true;
}

bool DXButton::Create(Vector3 pos, float width, float height, DXTexture* normalImg, DXTexture* hoverImg, DXTexture* pressedImg, DXTexture* disableImg, Vector4 normalColor, Vector4 hoverColor, Vector4 pressedColor, Vector4 disableColor)
{
    if (IsCreated)
    {
        return true;
    }

    if (width <= 0.0f || height <= 0.0f || pos.x <= 0.0f || pos.y <= 0.0f)
    {
        return false;
    }

    Position = pos;
    Width = width;
    Height = height;
    NormalImage = normalImg;
    HoverImage = hoverImg;
    PressedImage = pressedImg;
    DisableImage = disableImg;
    NormalColor = normalColor;
    HoverColor = hoverColor;
    PressedColor = pressedColor;
    DisableColor = disableColor;

    return Initialize();
}

void DXButton::SetVertexColor(Vector4 color)
{
    for (auto& it : Vertices)
    {
        it.Color = color;
    }
}
