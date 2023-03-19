#include "EffectManager.h"

namespace EFFECTUTIL
{
	ParticleSystem::ParticleSystem()
	{
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
        for (auto it : m_pEmitterList)
        {
            it->update();
        }

		return true;
	}

    bool ParticleSystem::preRender(Matrix* pWorld, Matrix* pView, Matrix* pProj)
    {
        for (auto it : m_pEmitterList)
        {
            it->setMatrix(pWorld, pView, pProj);
        }

        return true;
    }

	bool ParticleSystem::render()
	{
        for (auto it : m_pEmitterList)
        {
            it->render();
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
        for (auto it : m_PPSMap)
        {
            it.second->release();
            delete it.second;
            it.second = nullptr;
        }

        m_PPSMap.clear();

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
        if (m_PPSMap.find(key) == m_PPSMap.end())
        {
            PARTICLESYSTEM_FILE loadFile;
            ZeroMemory(&loadFile, sizeof(loadFile));

            bool bRet = FILEIO_MGR.loadFile(wszEffectFilePath, &loadFile);

            if (bRet)
            {
                ParticleSystem* newPPS;
                newPPS = new ParticleSystem;

                int numEmit = sizeof(loadFile.emitters) / sizeof(POINTPARTICLEEMITTER_FILE);

                for (int i = 0; i < numEmit; i++)
                {
                    if (loadFile.emitters[i].wszEmitterName[0] != NULL)
                    {
                        ParticleEmitter* newE = new ParticleEmitter;
                        newE->setDevice(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
                        newE->setName(loadFile.emitters[i].wszEmitterName);

                        if (loadFile.emitters[i].wszSpriteName[0] != NULL)
                        {
                            if (loadFile.emitters[i].iSpriteType == SPRITE_TYPE::SPRITE_UV)
                            {
                                if (!SPRITE_MGR.getUVPtr(loadFile.emitters[i].wszSpriteName))
                                {
                                    newE->release();
                                    delete newE;
                                    newE = nullptr;

                                    newPPS->release();
                                    delete newPPS;
                                    newPPS = nullptr;
                                    return false;
                                }

                                newE->createUVSpriteEmitter(loadFile.emitters[i].eProp,
                                    loadFile.emitters[i].wszSpriteName);
                            }
                            else
                            {
                                if (!SPRITE_MGR.getMTPtr(loadFile.emitters[i].wszSpriteName))
                                {
                                    newE->release();
                                    delete newE;
                                    newE = nullptr;

                                    newPPS->release();
                                    delete newPPS;
                                    newPPS = nullptr;
                                    return false;
                                }

                                newE->createMTSpriteEmitter(loadFile.emitters[i].eProp,
                                    loadFile.emitters[i].wszSpriteName);
                            }

                            newE->setRenderOption(loadFile.emitters[i].iBlendStateOption,
                                loadFile.emitters[i].bEnableDepth,
                                loadFile.emitters[i].bEnableDepthWrite);

                            newE->init();

                            newPPS->m_pEmitterList.push_back(newE);
                        }
                        else
                        {
                            DXTextureManager::GetInstance()->Load(loadFile.emitters[i].wszTexturePath);

                            newE->createEmitter(loadFile.emitters[i].eProp,
                                loadFile.emitters[i].wszTexturePath);

                            newE->setRenderOption(loadFile.emitters[i].iBlendStateOption,
                                loadFile.emitters[i].bEnableDepth,
                                loadFile.emitters[i].bEnableDepthWrite);

                            newE->init();

                            newPPS->m_pEmitterList.push_back(newE);
                        }
                    }
                }

                m_PPSMap.insert(std::make_pair(key, newPPS));
            }

            return bRet;
        }
        else { return true; }
	}

	ParticleSystem* EffectManager::getPointParticleEffect(std::wstring key)
	{
        key += L".PSystem";
        auto it = m_PPSMap.find(key);
        if (it != m_PPSMap.end())
        {
            return it->second;
        }

		return nullptr;
	}
}