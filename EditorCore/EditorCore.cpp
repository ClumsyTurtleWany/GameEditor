#include "EditorCore.h"
#include "DXUserInterfaceManager.h"

EditorCore::EditorCore()
{
}

EditorCore::~EditorCore()
{
}

bool EditorCore::Initialize()
{
    if (!DXUserInterfaceManager::GetInstance()->Initialize())
    {
        OutputDebugString(L"EditorCore::Initialize::DXUserInterfaceManager Failed Initialize.");
        return false;
    }

    if (!CreateVertexShader())
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.");
        return false;
    }

    if (!CreatePixelShader())
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.");
        return false;
    }

    if (!CreateInputLayout())
    {
        OutputDebugString(L"EditorCore::Initialize::CreateInputLayout::Failed Create Input Layout.");
        return false;
    }



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

bool EditorCore::CreateInputLayout()
{
    ID3DBlob* staticMeshVSCode = DXShaderManager::GetInstance()->GetVSCode(L"StaticMesh");
    D3D11_INPUT_ELEMENT_DESC staticMeshInputDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12 == float * 3 // Vertex의 Color 시작 바이트.
        {"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0} // 28 == float * 28 // Vertex의 Texture 시작 바이트.
    };
    UINT staticMeshInputElementNum = sizeof(staticMeshInputDescs) / sizeof(staticMeshInputDescs[0]);
    DXShaderManager::GetInstance()->CreateInputLayout(staticMeshVSCode, staticMeshInputDescs, staticMeshInputElementNum, L"StaticMesh");
   
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
    UINT skeletalMeshInputElementNum = sizeof(skeletalMeshInputDescs) / sizeof(skeletalMeshInputDescs[0]);
    DXShaderManager::GetInstance()->CreateInputLayout(skeletalMeshVSCode, skeletalMeshInputDescs, skeletalMeshInputElementNum, L"SkeletalMesh");
    

    return true;
}

bool EditorCore::CreateVertexShader()
{
    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../include/EditorCore/VS_StaticMesh.hlsl", L"StaticMesh"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../include/EditorCore/VS_StaticMesh.hlsl).");
    }

    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../include/EditorCore/VS_SkeletalMesh.hlsl", L"SkeletalMesh"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../include/EditorCore/VS_SkeletalMesh.hlsl)");
    }

    return true;
}

bool EditorCore::CreatePixelShader()
{
    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../include/EditorCore/PS_Light.hlsl", L"Light"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../include/EditorCore/PS_Light.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../include/EditorCore/PS_Landscape.hlsl", L"Landscape"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../include/EditorCore/PS_Landscape.hlsl)");
    }

    return true;
}

bool EditorCore::CreateComputeShader()
{
    if (!DXShaderManager::GetInstance()->CreateComputeShader(L"../include/EditorCore/CS_LandscapeSplatting.hlsl", L"Splatting"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateComputeShader::Failed Create Compute Shader.(../include/EditorCore/CS_LandscapeSplatting.hlsl).");
    }
    return true;
}

bool EditorCore::CreateGeometryShader()
{
    return true;
}
