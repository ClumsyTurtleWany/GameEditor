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
        if (!m_bPendingDelete)
        {
            m_fCurTime += Timer::GetInstance()->GetDeltaTime();

            if (m_PSProp.bLoop)
            {
                for (auto it : m_pEmitterList)
                {
                    it->update();
                }
            }
            else
            {
                if (m_fCurTime >= m_PSProp.fDuration + m_PSProp.fStDelay)
                {
                    m_bPendingDelete = true;
                }
                else if (m_fCurTime < m_PSProp.fDuration + m_PSProp.fStDelay)
                {
                    for (auto it : m_pEmitterList)
                    {
                        it->update();
                    }
                }
            }
        }

		return true;
	}

    bool ParticleSystem::preRender(Matrix* pWorld, Matrix* pView, Matrix* pProj)
    {
        if (!m_bPendingDelete)
        {
            for (auto it : m_pEmitterList)
            {
                it->setMatrix(pWorld, pView, pProj);
            }
        }

        return true;
    }

	bool ParticleSystem::render()
	{
        if (!m_bPendingDelete)
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
            PARTICLESYSTEM_FILE loadFile;
            ZeroMemory(&loadFile, sizeof(loadFile));

            bool bRet = FILEIO_MGR.loadFile(wszEffectFilePath, &loadFile);

            if (bRet)
            {
                m_PSFileMap.insert(std::make_pair(key, loadFile));

                return bRet;
            }
            else { return false; }
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

            int numEmit = sizeof(it->second.emitters) / sizeof(POINTPARTICLEEMITTER_FILE);

            for (int i = 0; i < numEmit; i++)
            {
                if (it->second.emitters[i].wszEmitterName[0] != NULL)
                {
                    ParticleEmitter* newE = new ParticleEmitter;
                    newE->setDevice(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
                    newE->setName(it->second.emitters[i].wszEmitterName);

                    if (it->second.emitters[i].wszSpriteName[0] != NULL)
                    {
                        if (it->second.emitters[i].iSpriteType == SPRITE_TYPE::SPRITE_UV)
                        {
                            if (!SPRITE_MGR.getUVPtr(it->second.emitters[i].wszSpriteName))
                            {
                                newE->release();
                                delete newE;
                                newE = nullptr;

                                newPPS->release();
                                delete newPPS;
                                newPPS = nullptr;
                                return nullptr;
                            }

                            newE->createUVSpriteEmitter(it->second.emitters[i].eProp,
                                it->second.emitters[i].wszSpriteName);
                        }
                        else
                        {
                            if (!SPRITE_MGR.getMTPtr(it->second.emitters[i].wszSpriteName))
                            {
                                newE->release();
                                delete newE;
                                newE = nullptr;

                                newPPS->release();
                                delete newPPS;
                                newPPS = nullptr;
                                return nullptr;
                            }

                            newE->createMTSpriteEmitter(it->second.emitters[i].eProp,
                                it->second.emitters[i].wszSpriteName);
                        }

                        newE->setRenderOption(it->second.emitters[i].iBlendStateOption,
                            it->second.emitters[i].bEnableDepth,
                            it->second.emitters[i].bEnableDepthWrite);

                        newE->init();

                        newPPS->m_pEmitterList.push_back(newE);
                    }
                    else
                    {
                        DXTextureManager::GetInstance()->Load(it->second.emitters[i].wszTexturePath);

                        newE->createEmitter(it->second.emitters[i].eProp,
                            it->second.emitters[i].wszTexturePath);

                        newE->setRenderOption(it->second.emitters[i].iBlendStateOption,
                            it->second.emitters[i].bEnableDepth,
                            it->second.emitters[i].bEnableDepthWrite);

                        newE->init();

                        newPPS->m_pEmitterList.push_back(newE);
                    }
                }
            }

            m_pPSSet.insert(newPPS);
            return newPPS;
        }

		return nullptr;
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