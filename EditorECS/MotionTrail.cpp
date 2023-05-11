#include "MotionTrail.h"

void updateConstBuf(void* pResource, size_t iByteSize, ID3D11Buffer* pBuf, ID3D11DeviceContext* pDContext)
{
    if (pDContext)
    {
        D3D11_MAPPED_SUBRESOURCE ms = {};

        HRESULT hr = pDContext->Map(pBuf,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &ms);

        if (SUCCEEDED(hr))
        {
            memcpy(ms.pData, pResource, iByteSize);

            pDContext->Unmap(pBuf, 0);
        }
    }
}

HRESULT MotionTrail::create(ID3D11Device* pDevice, 
    ID3D11DeviceContext* pDContext, 
    ECS::World* pMainWorld, 
    ECS::Entity* pTarget, 
    Camera* pMainCam, 
    int iTrailNum, 
    float fTrailDuration, 
    float fTrailSpawnTime)
{
    m_pDevice = pDevice;
    m_pDContext = pDContext;

    m_pMainWorld = pMainWorld;

    m_pTargetEntity = pTarget;
    m_pTargetMesh = m_pTargetEntity->GetComponent<SkeletalMeshComponent>();
    m_pTargetAnim = m_pTargetEntity->GetComponent<AnimationComponent>();

    m_Copy = *m_pTargetMesh;

    m_pMainCam = pMainCam;

    m_iTrailNum = iTrailNum;
    m_fTrailDuration = fTrailDuration;
    m_fTrailSpawnTime = fTrailSpawnTime;

    m_validate.resize(m_iTrailNum);
    m_trailData.resize(m_iTrailNum);
    m_pAnimArray.resize(m_iTrailNum);

    for (int i = 0; i < m_validate.size(); i++)
    {
        m_validate[i] = false;
    }

    for (int i = 0; i < m_pAnimArray.size(); i++)
    {
        m_pAnimArray[i] = new AnimationComponent;
    }

    m_pCBuf_TRAIL.resize(m_iTrailNum);

    ZeroMemory(m_trailData.data(), m_trailData.size() * sizeof(CBUF_MOTION_TRAIL_DATA));

    HRESULT hr = S_OK;

    for (int i = 0; i < m_iTrailNum; i++)
    {
        hr = EFFECTUTIL::createDXSimpleBuf(m_pDevice,
            sizeof(CBUF_MOTION_TRAIL_DATA),
            &m_trailData[i],
            D3D11_BIND_CONSTANT_BUFFER,
            &m_pCBuf_TRAIL[i]);

        m_trailData[i].timer = m_fTrailDuration;
        m_trailData[i].hitColorDuration = m_fTrailDuration;
    }

    m_spawnTimer = 0.0f;
    m_iInitTargetTrailIdx = 0;

    return hr;
}

bool MotionTrail::targetChange(ECS::Entity* pTarget)
{
    if (pTarget)
    {
        for (int i = 0; i < m_validate.size(); i++)
        {
            m_validate[i] = false;
        }

        m_pTargetEntity = pTarget;
        m_pTargetMesh = m_pTargetEntity->GetComponent<SkeletalMeshComponent>();
        m_pTargetAnim = m_pTargetEntity->GetComponent<AnimationComponent>();

        m_Copy = *m_pTargetMesh;
    }

    return true;
}

bool MotionTrail::render(float dt)
{
    m_spawnTimer += dt;

    TransformComponent* trans = m_pTargetEntity->GetComponent<TransformComponent>();
    MovementComponent* movement = m_pTargetEntity->GetComponent<MovementComponent>();

    m_vForward = movement->Forward;

    if (m_spawnTimer > m_fTrailSpawnTime && m_bEnable)
    {
        m_spawnTimer -= m_fTrailSpawnTime;

        if (m_iInitTargetTrailIdx >= m_trailData.size())
        {
            m_iInitTargetTrailIdx = 0;
        }

        m_validate[m_iInitTargetTrailIdx] = true;
        *m_pAnimArray[m_iInitTargetTrailIdx] = *m_pTargetAnim;
        m_trailData[m_iInitTargetTrailIdx].pos = trans->Translation;
        m_trailData[m_iInitTargetTrailIdx].direction = m_vForward;
        m_trailData[m_iInitTargetTrailIdx++].timer = 0.0f;
    }

    for (int i = 0; i < m_trailData.size(); i++)
    {
        m_trailData[i].timer += dt;

        updateConstBuf(&m_trailData[i], sizeof(CBUF_MOTION_TRAIL_DATA), m_pCBuf_TRAIL[i].Get(), m_pDContext);

        if (m_validate[i])
        {
            TransformComponent world;
            world.Translation = m_trailData[i].pos -m_trailData[i].direction * m_trailData[i].timer * 2.0f;
            world.Rotation = trans->Rotation;
            world.Scale = trans->Scale;

            m_pAnimArray[i]->UpdateAnim(&m_Copy, 0.0f);

            m_pDContext->PSSetConstantBuffers(6, 1, m_pCBuf_TRAIL[i].GetAddressOf());

            m_Copy.UpdateTransformMatrix(world);
            m_Copy.RenderTrail();
        }
    }

    return true;
}

bool MotionTrail::release()
{
    for (int i = 0; i < m_pAnimArray.size(); i++)
    {
        if (m_pAnimArray[i])
        {
            delete m_pAnimArray[i];
            m_pAnimArray[i] = nullptr;
        }
    }

    m_pAnimArray.clear();

    for (int i = 0; i < m_validate.size(); i++)
    {
        m_validate[i] = false;
    }

    return true;
}