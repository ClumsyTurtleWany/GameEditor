#include "EffectManager.h"

namespace EFFECTUTIL
{
	ParticleSystem::ParticleSystem()
	{
        m_fCurTime = 0.0f;

        m_bPendingDelete = false;
	}

	ParticleSystem::~ParticleSystem()
	{
	}

    void ParticleSystem::setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
    {
        for (auto it : m_pEmitterList)
        {
            it->setDevice(pDevice, pDContext);
        }
    }

	bool ParticleSystem::init()
	{
        for (auto it : m_pEmitterList)
        {
            it->init();
        }

		return true;
	}

	bool ParticleSystem::update()
	{
        if (m_PSProp.bShow)
        {
            float dt = Timer::GetInstance()->SecondPerFrame * m_PSProp.fPlaySpeed;
            m_fCurTime += dt;

            if (m_fCurTime < m_PSProp.fDuration + m_PSProp.fStDelay)
            {
                if (m_fCurTime >= m_PSProp.fStDelay)
                {
                    for (auto it : m_pEmitterList)
                    {
                        it->update(dt);
                    }
                }
            }
            else
            {
                if (m_PSProp.bLoop)
                {
                    m_fCurTime = 0.0f;
                }
                else
                {
                    m_bPendingDelete = true;
                }
            }
        }

		return true;
	}

    void ParticleSystem::setMatrix(Matrix* pWorld, Matrix* pView, Matrix* pProj, Matrix* pParentWorld)
    {
        if (m_PSProp.bShow && !m_bPendingDelete)
        {
            if (pWorld)         { m_matLocalWorld = *pWorld; }
            if (pView)          { m_matView = *pView; }
            if (pProj)          { m_matProj = *pProj; }
            if (pParentWorld)   { m_matParentWorld = *pParentWorld; }

             m_matWorld = m_matLocalWorld * m_matParentWorld;

            for (auto it : m_pEmitterList)
            {
                it->setMatrix(nullptr, pView, pProj, &m_matWorld);
            }
        }
    }

	bool ParticleSystem::render()
	{
        if (m_PSProp.bShow && !m_bPendingDelete)
        {
            for (auto it : m_pEmitterList)
            {
                it->render();
            }
        }

		return true;
	}

	bool ParticleSystem::release()
	{
        for (auto it : m_pEmitterList)
        {
            it->release();
            delete it;
        }

		return true;
	}

    void ParticleSystem::setProp(ParticlesystemProperty PSProp)
    {
        m_PSProp = PSProp;

        for (auto it : m_pEmitterList)
        {
            it->m_playSpeed = PSProp.fPlaySpeed;
        }
    }

    void ParticleSystem::toggleActivation(bool bActivate)
    {
        for (auto it : m_pEmitterList)
        {
            it->m_bActivate = bActivate;
        }
    }

	EffectManager::EffectManager()
	{
	}

	EffectManager::~EffectManager()
	{
	}

	bool EffectManager::init()
	{
        m_wszDefaultPointParticleFilePath = L"../resource/EffectPart/save/particlesystem/";
        loadDefaultEffectDir();

		return true;
	}

	bool EffectManager::release()
	{
        for (auto it : m_PSFileMap)
        {
            delete it.second;
            it.second = nullptr;
        }

        m_PSFileMap.clear();

        for (auto it : m_pPSSet)
        {
            it->release();
            delete it;
            it = nullptr;
        }

        m_wszDefaultPointParticleFilePath.clear();

		return true;
	}

	bool EffectManager::loadDefaultEffectDir()
	{
        std::vector<std::wstring> inExtArr;
        inExtArr.push_back(L".PSystem");

        std::vector<std::wstring> outPathArr;
        std::vector<std::wstring> outNameArr;

        loadDir(m_wszDefaultPointParticleFilePath, &inExtArr, &outPathArr, &outNameArr);

        for (int i = 0; i < outPathArr.size(); i++)
        {
            loadPointParticleEffect(outPathArr[i], outNameArr[i]);
        }

		return true;
	}

	bool EffectManager::loadPointParticleEffect(std::wstring wszEffectFilePath, std::wstring key)
	{
        if (m_PSFileMap.find(key) == m_PSFileMap.end())
        {
            PARTICLESYSTEM_FILE* pLoadFile = new PARTICLESYSTEM_FILE;
            ZeroMemory(pLoadFile, sizeof(PARTICLESYSTEM_FILE));

            bool bRet = FILEIO_MGR.loadFile(wszEffectFilePath, pLoadFile);

            if (bRet) 
            {
                m_PSFileMap.insert(std::make_pair(key, pLoadFile));
            }
            else
            {
                delete pLoadFile;
            }
            
            return bRet;

        }
        else { return true; }
	}

	ParticleSystem* EffectManager::getPointParticleEffect(std::wstring key)
	{
        key += L".PSystem";
        auto it = m_PSFileMap.find(key);
        if (it != m_PSFileMap.end())
        {
            ParticleSystem* newPPS;
            newPPS = new ParticleSystem;

            int numEmit = it->second->iEmiiterCnt;
            bool bRet = true;

            for (int i = 0; i < numEmit; i++)
            {
                if (it->second->emitters[i].wszEmitterName[0] != NULL)
                {
                    ParticleEmitter* pNew = nullptr;

                    switch (it->second->emitters[i].emitterType)
                    {
                        case EFFECT_EMITTER_TYPE::EET_DEFAULT :
                        {
                            bRet = createDefaultEmitter(&pNew, it->second->emitters[i]);

                            if (!bRet)
                            {
                                newPPS->release();
                                delete newPPS;
                                newPPS = nullptr;

                                return nullptr;
                            }

                            newPPS->m_pEmitterList.push_back(pNew);
                        } break;

                        case EFFECT_EMITTER_TYPE::EET_TRAIL:
                        {
                            bRet = createTrailEmitter(&pNew, it->second->emitters[i]);

                            if (!bRet)
                            {
                                newPPS->release();
                                delete newPPS;
                                newPPS = nullptr;

                                return nullptr;
                            }

                            newPPS->m_pEmitterList.push_back(pNew);
                        } break;

                        case EFFECT_EMITTER_TYPE::EET_BURST:
                        {
                            bRet = createBurstEmitter(&pNew, it->second->emitters[i]);

                            if (!bRet)
                            {
                                newPPS->release();
                                delete newPPS;
                                newPPS = nullptr;

                                return nullptr;
                            }

                            newPPS->m_pEmitterList.push_back(pNew);
                        } break;
                    }
                }
            }

            m_pPSSet.insert(newPPS);
            return newPPS;
        }

		return nullptr;
	}

    bool EffectManager::createDefaultEmitter(ParticleEmitter** ppOut, POINTPARTICLEEMITTER_FILE& eFile)
    {
        ParticleEmitter* newE = new ParticleEmitter;

        newE->setDevice(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
        newE->setName(eFile.wszEmitterName);

        if (eFile.wszSpriteName[0] != NULL)
        {
            if (eFile.iSpriteType == SPRITE_TYPE::SPRITE_UV)
            {
                if (!SPRITE_MGR.getUVPtr(eFile.wszSpriteName))
                {
                    newE->release();
                    delete newE;
                    newE = nullptr;

                    return false;
                }

                newE->createUVSpriteEmitter(eFile.eProp,
                    eFile.wszSpriteName);
            }
            else
            {
                if (!SPRITE_MGR.getMTPtr(eFile.wszSpriteName))
                {
                    newE->release();
                    delete newE;
                    newE = nullptr;

                    return false;
                }

                newE->createMTSpriteEmitter(eFile.eProp,
                    eFile.wszSpriteName);
            }

            newE->setRenderOption(eFile.iBlendStateOption,
                eFile.bEnableDepth,
                eFile.bEnableDepthWrite,
                eFile.bBackCull);

            newE->init();
        }
        else
        {
            DXTextureManager::GetInstance()->Load(eFile.wszTexturePath);

            newE->createEmitter(eFile.eProp,
                eFile.wszTexturePath);

            newE->setRenderOption(eFile.iBlendStateOption,
                eFile.bEnableDepth,
                eFile.bEnableDepthWrite,
                eFile.bBackCull);

            newE->init();
        }

        *ppOut = newE;
        return true;
    }

    bool EffectManager::createTrailEmitter(ParticleEmitter** ppOut, POINTPARTICLEEMITTER_FILE& eFile)
    {
        TrailEmitter* newE = new TrailEmitter;

        newE->setDevice(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
        newE->setName(eFile.wszEmitterName);

        if (eFile.wszSpriteName[0] != NULL)
        {
            if (eFile.iSpriteType == SPRITE_TYPE::SPRITE_UV)
            {
                if (!SPRITE_MGR.getUVPtr(eFile.wszSpriteName))
                {
                    newE->release();
                    delete newE;
                    newE = nullptr;

                    return false;
                }

                newE->createUVSpriteEmitter(eFile.eProp,
                    eFile.wszSpriteName);
            }
            else
            {
                if (!SPRITE_MGR.getMTPtr(eFile.wszSpriteName))
                {
                    newE->release();
                    delete newE;
                    newE = nullptr;

                    return false;
                }

                newE->createMTSpriteEmitter(eFile.eProp,
                    eFile.wszSpriteName);
            }

            newE->setRenderOption(eFile.iBlendStateOption,
                eFile.bEnableDepth,
                eFile.bEnableDepthWrite,
                eFile.bBackCull);

            newE->init();
        }
        else
        {
            DXTextureManager::GetInstance()->Load(eFile.wszTexturePath);

            newE->createEmitter(eFile.eProp,
                eFile.wszTexturePath);

            newE->setRenderOption(eFile.iBlendStateOption,
                eFile.bEnableDepth,
                eFile.bEnableDepthWrite,
                eFile.bBackCull);

            newE->init();
        }

        *ppOut = newE;
        return true;
    }

    bool EffectManager::createBurstEmitter(ParticleEmitter** ppOut, POINTPARTICLEEMITTER_FILE& eFile)
    {
        BurstEmitter* newE = new BurstEmitter;

        newE->setDevice(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
        newE->setName(eFile.wszEmitterName);

        if (eFile.wszSpriteName[0] != NULL)
        {
            if (eFile.iSpriteType == SPRITE_TYPE::SPRITE_UV)
            {
                if (!SPRITE_MGR.getUVPtr(eFile.wszSpriteName))
                {
                    newE->release();
                    delete newE;
                    newE = nullptr;

                    return false;
                }

                newE->createUVSpriteEmitter(eFile.eProp,
                    eFile.wszSpriteName);
            }
            else
            {
                if (!SPRITE_MGR.getMTPtr(eFile.wszSpriteName))
                {
                    newE->release();
                    delete newE;
                    newE = nullptr;

                    return false;
                }

                newE->createMTSpriteEmitter(eFile.eProp,
                    eFile.wszSpriteName);
            }

            newE->setRenderOption(eFile.iBlendStateOption,
                eFile.bEnableDepth,
                eFile.bEnableDepthWrite,
                eFile.bBackCull);

            newE->init();
        }
        else
        {
            DXTextureManager::GetInstance()->Load(eFile.wszTexturePath);

            newE->createEmitter(eFile.eProp,
                eFile.wszTexturePath);

            newE->setRenderOption(eFile.iBlendStateOption,
                eFile.bEnableDepth,
                eFile.bEnableDepthWrite,
                eFile.bBackCull);

            newE->init();
        }

        *ppOut = newE;
        return true;
    }

    bool EffectManager::erase(ParticleSystem* pTarget)
    {
        auto it = m_pPSSet.find(pTarget);

        if (it != m_pPSSet.end())
        {
            ParticleSystem* temp = (*it);
            m_pPSSet.erase(it);
            temp->release();
            delete temp;
            return true;
        }

        return false;
    }
}