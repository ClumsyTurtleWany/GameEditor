#include "EditorCore.h"
#include "DXUserInterfaceManager.h"

bool EditorCore::Initialize()
{
    DXUserInterfaceManager::GetInstance()->SetContext(Device.m_pImmediateContext);
    if (!DXUserInterfaceManager::GetInstance()->Initialize())
    {
        OutputDebugString(L"EditorCore::Initialize::DXUserInterfaceManager Failed Initialize.");
        return false;
    }

    DXShaderManager::GetInstance()->LoadVSCode(L"../include/EditorCore/VS_StaticMesh.hlsl", L"StaticMesh");
    ID3DBlob* staticMeshVSCode = DXShaderManager::GetInstance()->GetVSCode(L"StaticMesh");
    D3D11_INPUT_ELEMENT_DESC staticMeshInputDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12 == float * 3 // Vertex의 Color 시작 바이트.
        {"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0} // 28 == float * 28 // Vertex의 Texture 시작 바이트.
    };
    DXShaderManager::GetInstance()->CreateInputLayout(staticMeshVSCode, staticMeshInputDescs, L"StaticMesh");
    DXShaderManager::GetInstance()->CreateVertexShader(staticMeshVSCode, L"StaticMesh");

    DXShaderManager::GetInstance()->LoadVSCode(L"../include/EditorCore/VS_SkeletalMesh.hlsl", L"SkeletalMesh");
    ID3DBlob* skeletalMeshVSCode = DXShaderManager::GetInstance()->GetVSCode(L"SkeletalMesh");
    D3D11_INPUT_ELEMENT_DESC skeletalMeshInputDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12 == float * 3 // Vertex의 Color 시작 바이트.
        {"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 28 == float * 28 // Vertex의 Texture 시작 바이트.

        {"INDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    DXShaderManager::GetInstance()->CreateInputLayout(skeletalMeshVSCode, skeletalMeshInputDescs, L"SkeletalMesh");
    DXShaderManager::GetInstance()->CreateVertexShader(staticMeshVSCode, L"SkeletalMesh");

    return true;
}

bool EditorCore::Frame()
{
    DXUserInterfaceManager::GetInstance()->Frame();

    return true;
}

bool EditorCore::Render()
{
    DXUserInterfaceManager::GetInstance()->Render();

    return true;
}

bool EditorCore::Release()
{
    DXUserInterfaceManager::GetInstance()->Release();

    return true;
}
