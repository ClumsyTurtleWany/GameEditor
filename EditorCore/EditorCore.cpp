#include "EditorCore.h"

EditorCore::EditorCore()
{
}

EditorCore::~EditorCore()
{
}

bool EditorCore::Initialize()
{
    if (!CreateVertexShader())
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.");
        return false;
    }

    if (!CreateGeometryShader())
    {
        OutputDebugString(L"EditorCore::Initialize::CreateGeometryShader::Failed Create Geometry Shader.");
        return false;
    }

    if (!CreatePixelShader())
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.");
        return false;
    }

    if (!CreateComputeShader())
    {
        OutputDebugString(L"EditorCore::Initialize::CreateComputeShader::Failed Create Compute Shader.");
        return false;
    }

    if (!CreateInputLayout())
    {
        OutputDebugString(L"EditorCore::Initialize::CreateInputLayout::Failed Create Input Layout.");
        return false;
    }

    if (!MaterialManager::GetInstance()->Initialize())
    {
        OutputDebugString(L"EditorCore::Initialize::MaterialManager::Failed Initialize Material Manager.");
        return false;
    }

    if (!DXRenderTargetManager::GetInstance()->Initialize())
    {
        OutputDebugString(L"EditorCore::Initialize::DXRenderTargetManager::Failed Initialize Render Target Manager.");
        return false;
    }

    return true;
}

bool EditorCore::Frame()
{
    MAIN_PICKER.Update();
    return true;
}

bool EditorCore::Render()
{
    return true;
}

bool EditorCore::Release()
{
    if (!DXRenderTargetManager::GetInstance()->Release())
    {
        OutputDebugString(L"EditorCore::Release::DXRenderTargetManager::Failed Release Render Target Manager.");
    }

    if (!MaterialManager::GetInstance()->Release())
    {
        OutputDebugString(L"EditorCore::Release::MaterialManager::Failed Release Material Manager.");
    }

    return true;
}

bool EditorCore::CreateInputLayout()
{
    //------------------------------------------------------------------------------------
    // Static Mesh Input Layout
    //------------------------------------------------------------------------------------
    ID3DBlob* staticMeshVSCode = DXShaderManager::GetInstance()->GetVSCode(L"StaticMesh");
    D3D11_INPUT_ELEMENT_DESC staticMeshInputDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12 == float * 3 // Vertex의 Color 시작 바이트.
        {"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 28 == float * 28 // Vertex의 Texture 시작 바이트.
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    UINT staticMeshInputElementNum = sizeof(staticMeshInputDescs) / sizeof(staticMeshInputDescs[0]);
    DXShaderManager::GetInstance()->CreateInputLayout(staticMeshVSCode, staticMeshInputDescs, staticMeshInputElementNum, L"StaticMesh");
   
    //------------------------------------------------------------------------------------
    // Skeletal Mesh Input Layout
    //------------------------------------------------------------------------------------
    ID3DBlob* skeletalMeshVSCode = DXShaderManager::GetInstance()->GetVSCode(L"SkeletalMesh");
    D3D11_INPUT_ELEMENT_DESC skeletalMeshInputDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12 == float * 3 // Vertex의 Color 시작 바이트.
        {"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 28 == float * 28 // Vertex의 Texture 시작 바이트.
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},

        {"INDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT skeletalMeshInputElementNum = sizeof(skeletalMeshInputDescs) / sizeof(skeletalMeshInputDescs[0]);
    DXShaderManager::GetInstance()->CreateInputLayout(skeletalMeshVSCode, skeletalMeshInputDescs, skeletalMeshInputElementNum, L"SkeletalMesh");

    //------------------------------------------------------------------------------------
    // ParticleEffect Input Layout
    //------------------------------------------------------------------------------------
    ID3DBlob* particleEffectVSCode = DXShaderManager::GetInstance()->GetVSCode(L"VS_PointParticle");
    D3D11_INPUT_ELEMENT_DESC particleEffectInputDescs[] =
    {
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 12,  D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 24,  D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,        0, 40,  D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "SPRITERT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 48,  D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "ROTATION",   0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 64,  D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "ROTATION",   1, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 80,  D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "ROTATION",   2, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 96,  D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "ROTATION",   3, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 112, D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "SCALE",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 128,  D3D11_INPUT_PER_VERTEX_DATA, NULL}
    };
    UINT particleEffectInputElementNum = sizeof(particleEffectInputDescs) / sizeof(particleEffectInputDescs[0]);
    DXShaderManager::GetInstance()->CreateInputLayout(particleEffectVSCode, particleEffectInputDescs, particleEffectInputElementNum, L"VL_PointParticle");

    //------------------------------------------------------------------------------------
    // Line Object Input Layout
    //------------------------------------------------------------------------------------
    ID3DBlob* lineObjVSCode = DXShaderManager::GetInstance()->GetVSCode(L"VS_LineObject");
    D3D11_INPUT_ELEMENT_DESC lineObjInputDescs[] =
    {
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D11_INPUT_PER_VERTEX_DATA, NULL},
        { "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 12,  D3D11_INPUT_PER_VERTEX_DATA, NULL},
    };
    UINT lineObjInputElementNum = sizeof(lineObjInputDescs) / sizeof(lineObjInputDescs[0]);
    DXShaderManager::GetInstance()->CreateInputLayout(lineObjVSCode, lineObjInputDescs, lineObjInputElementNum, L"VS_LineObject");

    return true;
}

bool EditorCore::CreateVertexShader()
{
    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/HLSL/VertexShader/VS_NormalMapStaticMesh.hlsl", L"StaticMesh"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/HLSL/VertexShader/VS_StaticMesh.hlsl).");
    }

    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/HLSL/VertexShader/VS_SkeletalMesh.hlsl", L"SkeletalMesh"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/HLSL/VertexShader/VS_SkeletalMesh.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/HLSL/VertexShader/VS_UserInterface.hlsl", L"UI"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/HLSL/VertexShader/VS_UserInterface.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/HLSL/VertexShader/VS_NormalMapStaticMesh.hlsl", L"NormalMapStaticMesh"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/HLSL/VertexShader/VS_NormalMapStaticMesh.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/HLSL/VertexShader/VS_Sky.hlsl", L"Sky"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/HLSL/VertexShader/VS_Sky.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/HLSL/VertexShader/VS_Shadow.hlsl", L"Shadow"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/HLSL/VertexShader/VS_Shadow.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/HLSL/VertexShader/VS_SkeletalShadow.hlsl", L"SkeletalShadow"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/HLSL/VertexShader/VS_SkeletalShadow.hlsl)");
    }

    //------------------------------------------------------------------------------------
    // Effect Vertex Shader
    //------------------------------------------------------------------------------------
    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/EffectPart/shader/VS_PointParticle.hlsl", L"VS_PointParticle"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/EffectPart/shader/VS_PointParticle.hlsl)");
    }

    //------------------------------------------------------------------------------------
    // Line Object Shader
    //------------------------------------------------------------------------------------
    if (!DXShaderManager::GetInstance()->CreateVertexShader(L"../resource/HLSL/VertexShader/VS_LineObject.hlsl", L"VS_LineObject"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateVertexShader::Failed Create Vertex Shader.(../resource/HLSL/VertexShader/VS_LineObject.hlsl)");
    }

    return true;
}

bool EditorCore::CreatePixelShader()
{
    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_Light.hlsl", L"Light"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_Light.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_Landscape.hlsl", L"Landscape"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_Landscape.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_TexturedUI.hlsl", L"T_UI"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_TexturedUI.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_NonTexturedUI.hlsl", L"NT_UI"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_NonTexturedUI.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_Default.hlsl", L"Default"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_Default.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_NormalMap.hlsl", L"NormalMap"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_NormalMap.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_Shadow.hlsl", L"Shadow"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_Shadow.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_Texture2DArray.hlsl", L"2DArray"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_Texture2DArray.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_Sky.hlsl", L"Sky"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_Sky.hlsl)");
    }

    //------------------------------------------------------------------------------------
    // Effect Pixel Shader
    //------------------------------------------------------------------------------------
    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/EffectPart/shader/PS_PointParticle_Alphablend.hlsl", L"PS_PointParticle_Alphablend"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/EffectPart/shader/PS_PointParticle_Alphablend.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/EffectPart/shader/PS_PointParticle_Alphatest.hlsl", L"PS_PointParticle_Alphatest"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/EffectPart/shader/PS_PointParticle_Alphatest.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/EffectPart/shader/PS_PointParticle_Dualsource.hlsl", L"PS_PointParticle_Dualsource"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/EffectPart/shader/PS_PointParticle_Dualsource.hlsl)");
    }

    //------------------------------------------------------------------------------------
    // Line Object Shader
    //------------------------------------------------------------------------------------
    if (!DXShaderManager::GetInstance()->CreatePixelShader(L"../resource/HLSL/PixelShader/PS_LineObject.hlsl", L"PS_LineObject"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreatePixelShader::Failed Create Pixel Shader.(../resource/HLSL/PixelShader/PS_LineObject.hlsl)");
    }

    return true;
}

bool EditorCore::CreateComputeShader()
{
    if (!DXShaderManager::GetInstance()->CreateComputeShader(L"../resource/HLSL/ComputeShader/CS_LandscapeSplatting.hlsl", L"Splatting"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateComputeShader::Failed Create Compute Shader.(../resource/HLSL/ComputeShader/CS_LandscapeSplatting.hlsl).");
    }
    return true;
}

bool EditorCore::CreateGeometryShader()
{
    //------------------------------------------------------------------------------------
    // Effect Geometry Shader
    //------------------------------------------------------------------------------------
    if (!DXShaderManager::GetInstance()->CreateGeometryShader(L"../resource/EffectPart/shader/GS_PointParticle.hlsl", L"GS_PointParticle"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateGeometryShader::Failed Create Geometry Shader.(../resource/EffectPart/shader/GS_PointParticle.hlsl)");
    }

    if (!DXShaderManager::GetInstance()->CreateGeometryShader(L"../resource/EffectPart/shader/GS_ParticleTrail.hlsl", L"GS_ParticleTrail"))
    {
        OutputDebugString(L"EditorCore::Initialize::CreateGeometryShader::Failed Create Geometry Shader.(../resource/EffectPart/shader/GS_ParticleTrail.hlsl)");
    }

    return true;
}
