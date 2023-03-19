#include "FileIOManager.h"

namespace EFFECTUTIL
{
    BYTE* FileIOManager::m_pfileBuf = 0;
    LARGE_INTEGER		FileIOManager::m_fileSize;
    DWORD				FileIOManager::m_numberOfFragment = 0;
    DWORD				FileIOManager::m_curOPCount = 0;
    HANDLE				FileIOManager::m_hOPStart = 0;
    HANDLE				FileIOManager::m_hProgressPrint = 0;

    FileIOManager::FileIOManager()
    {

    }

    FileIOManager::~FileIOManager()
    {
    }

    bool FileIOManager::init()
    {
        m_pfileBuf = 0;
        m_fileSize.QuadPart = 0;
        m_numberOfFragment = 0;
        m_curOPCount = 0;
        m_hOPStart = 0;
        m_hProgressPrint = 0;

        return true;
    }

    bool FileIOManager::release()
    {
        return true;
    }

    bool FileIOManager::loadFile(std::wstring wszFilePath, void* destLoadData)
    {
        bool bRet = true;

        init();

        HANDLE checkHandle = CreateFile(wszFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (checkHandle == INVALID_HANDLE_VALUE || checkHandle == NULL) { return false; }
        else { CloseHandle(checkHandle); }

        //�̺�Ʈ �ڵ� ����
        m_hOPStart = CreateEvent(0, FALSE, FALSE, nullptr);		//�ڵ� ���� �̺�Ʈ�� ����
        if (m_hOPStart == INVALID_HANDLE_VALUE || m_hOPStart == NULL)
        {
            int iRet = GetLastError();
            std::wstring errStr = L"Error Code : ";
            errStr += std::to_wstring(iRet) + L"\n";
            OutputDebugString(errStr.c_str());
            return false;
        }

        m_hProgressPrint = CreateEvent(0, FALSE, FALSE, nullptr);		//			"
        if (m_hProgressPrint == INVALID_HANDLE_VALUE || m_hProgressPrint == NULL)
        {
            int iRet = GetLastError();
            std::wstring errStr = L"Error Code : ";
            errStr += std::to_wstring(iRet) + L"\n";
            OutputDebugString(errStr.c_str());

            return false;
        }

        //������ ������
        UINT idRead;

        //��ȯ���� uintptr_t�� �������� �ڵ�ó�� ���ȴ�.
        uintptr_t hReadThread = _beginthreadex(nullptr, 0, asyncReadProc, (LPVOID)wszFilePath.c_str(), 0, &idRead);
        if (!hReadThread) { CloseHandle(m_hOPStart), CloseHandle(m_hProgressPrint); return false; }

        while (true)
        {
            //�̺�Ʈ ����Լ� : Ÿ�Ӿƿ� �ð��� �� �ð� ��ŭ ��ٸ� �̺�Ʈ �ڵ��� �����Ѵ�. 
            //�̺�Ʈ �ڵ��� ������ �Լ��� ����Ű�� �ڵ��̸� �̺�Ʈ �ڵ��� ����Ű�� �̺�Ʈ �Ǵ� �Լ��� �߻���Ų ��ȯ���� ���� �� �Լ��� ��ȯ���� �޶�����.
            //��ȯ��
                //�������� �̺�Ʈ ��� ��Ʈ : WAIT_OBJECT_0 => 0
                //Ÿ�Ӿƿ� : WAIT_TIMEOUT => 0x00000102L
                //���� : WAIT_FAILED => 0xFFFFFFFF

            if (WaitForSingleObject(m_hProgressPrint, INFINITE) == WAIT_OBJECT_0)
            {
                //���� ��Ȳ�� ������� ����Ѵ�.
                float fProgress = (float)m_curOPCount * 100.0f / m_numberOfFragment;

                std::wstring progressStr = L"In Progress : ";
                progressStr += std::to_wstring(fProgress) + L"%\n";
                OutputDebugString(progressStr.c_str());

                if (m_numberOfFragment == m_curOPCount) { break; }
            }
        }

        //������ ���Ḧ ��ٸ���.
        WaitForSingleObject((HANDLE)hReadThread, INFINITE);

        //�����尡 ����Ǹ� �ڵ��� �ݴ´�.
        CloseHandle((HANDLE)hReadThread);
        CloseHandle(m_hOPStart);
        CloseHandle(m_hProgressPrint);

        memcpy(destLoadData, m_pfileBuf, m_fileSize.LowPart);

        //���� �Ҵ��� ���� ���۸� �����Ѵ�.
        delete[] m_pfileBuf;

        return bRet;
    }

    bool FileIOManager::saveFile(std::wstring wszFilePath, void* srcSaveData, UINT iBytes)
    {
        init();

        //�̺�Ʈ �ڵ� ����
        m_hOPStart = CreateEvent(0, TRUE, FALSE, nullptr);		//�ڵ� ���� �̺�Ʈ�� ����
        if (m_hOPStart == INVALID_HANDLE_VALUE || m_hOPStart == NULL)
        {
            int iRet = GetLastError();
            std::wstring errStr = L"Error Code : ";
            errStr += std::to_wstring(iRet) + L"\n";
            OutputDebugString(errStr.c_str());
            return false;
        }

        m_hProgressPrint = CreateEvent(0, TRUE, FALSE, nullptr);		//			"
        if (m_hProgressPrint == INVALID_HANDLE_VALUE || m_hProgressPrint == NULL)
        {
            int iRet = GetLastError();
            std::wstring errStr = L"Error Code : ";
            errStr += std::to_wstring(iRet) + L"\n";
            OutputDebugString(errStr.c_str());

            return false;
        }

        m_pfileBuf = new BYTE[iBytes];
        memcpy(m_pfileBuf, srcSaveData, iBytes);
        m_fileSize.LowPart = iBytes;

        UINT idWrite;

        //��ȯ���� uintptr_t�� �������� �ڵ�ó�� ���ȴ�.
        uintptr_t hWriteThread = _beginthreadex(nullptr, 0, asyncWriteProc, (LPVOID)wszFilePath.c_str(), 0, &idWrite);
        if (!hWriteThread) { CloseHandle(m_hOPStart), CloseHandle(m_hProgressPrint); return false; }

        while (true)
        {
            //�̺�Ʈ ����Լ� : Ÿ�Ӿƿ� �ð��� �� �ð� ��ŭ ��ٸ� �̺�Ʈ �ڵ��� �����Ѵ�. 
            //�̺�Ʈ �ڵ��� ������ �Լ��� ����Ű�� �ڵ��̸� �̺�Ʈ �ڵ��� ����Ű�� �̺�Ʈ �Ǵ� �Լ��� �߻���Ų ��ȯ���� ���� �� �Լ��� ��ȯ���� �޶�����.
            //��ȯ��
                //�������� �̺�Ʈ ��� ��Ʈ : WAIT_OBJECT_0 => 0
                //Ÿ�Ӿƿ� : WAIT_TIMEOUT => 0x00000102L
                //���� : WAIT_FAILED => 0xFFFFFFFF
            if (WaitForSingleObject(m_hProgressPrint, INFINITE) == WAIT_OBJECT_0)
            {
                //���� ��Ȳ�� ������� ����Ѵ�.
                float fProgress = (float)m_curOPCount * 100.0f / m_numberOfFragment;

                std::wstring progressStr = L"In Progress : ";
                progressStr += std::to_wstring(fProgress) + L"%\n";
                OutputDebugString(progressStr.c_str());

                if (m_numberOfFragment == m_curOPCount) { break; }
            }
        }

        //������ ���Ḧ ��ٸ���.
        WaitForSingleObject((HANDLE)hWriteThread, INFINITE);

        //�����尡 ����Ǹ� �ڵ��� �ݴ´�.
        CloseHandle((HANDLE)hWriteThread);
        CloseHandle(m_hOPStart);
        CloseHandle(m_hProgressPrint);

        //���� �Ҵ��� ���� ���۸� �����Ѵ�.
        delete[] m_pfileBuf;
        m_pfileBuf = nullptr;

        return true;
    }

    UINT FileIOManager::asyncReadProc(LPVOID argList)
    {
        //���� �̸� ĳ����
        const WCHAR* wFilename = (WCHAR*)argList;

        //OVERLAPPED ����ü ���� : �񵿱� ���� �۾��� �۾� ���� ������ �����ϴ� �Ϸ��� ���ؽ�Ʈ ������ �����ϴ� ����ü
        OVERLAPPED readOV = { 0 };	//�ݵ�� �ʱ�ȭ �ؾ��Ѵ�.

        //���� ���Ͽ� ���� �ڵ��� �����Ѵ�. : �񵿱� �۾��� FILE_FLAG_OVERLAPPED�� ���Ҿ� OVERLAPPED����ü�� �ʿ��ϴ�.
        HANDLE hRead = CreateFile(wFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

        LARGE_INTEGER readBytes = { 0 };				//���� ����Ʈ ��
        DWORD dwOffset = MAX_NUMBER_OF_BYTE_TO_READ;

        //������ �ƴ϶�� ���� �б⸦ �����Ѵ�.
        if (hRead != INVALID_HANDLE_VALUE)
        {
            ::GetFileSizeEx(hRead, &m_fileSize);			//���� ũ�� �ҷ�����
            m_pfileBuf = new BYTE[m_fileSize.LowPart];		//���� ���� ����

            //��� ���� �Ǵ��� Ȯ��
            m_numberOfFragment = m_fileSize.QuadPart / MAX_NUMBER_OF_BYTE_TO_READ;
            if (m_fileSize.QuadPart % MAX_NUMBER_OF_BYTE_TO_READ) { m_numberOfFragment++; }

            while (true)
            {
                //�������� : ���� ����Ʈ ���� ��ü ����Ʈ ���� ���ų� Ŭ ��(�̶��� ��������)
                if (readBytes.QuadPart >= m_fileSize.QuadPart) { break; }

                //�б� �۾��� ���� �뷮�� �����¿� �ش��ϴ� ����Ʈ ����ŭ�� ���� �� �ִ��� Ȯ���Ѵ�.
                if (m_fileSize.QuadPart - readBytes.QuadPart < dwOffset) { dwOffset = m_fileSize.QuadPart - readBytes.QuadPart; }

                //�б� �۾��� �����Ѵ�. ���� �߻��� false�� ��ȯ�Ѵ�.
                if (!asyncReadSub(hRead, dwOffset, readBytes, readOV))
                {
                    CloseHandle(hRead);
                    return 0;
                }

                SetEvent(m_hProgressPrint);
                m_curOPCount++;
            }

            // ���� - ���� ������̵� �ڵ��� �ݴ´�.
            CloseHandle(hRead);
        }
        else
        {
            ResetEvent(m_hProgressPrint);
        }

        return readBytes.LowPart;									//���� ũ�� ��ȯ
    }

    UINT FileIOManager::asyncWriteProc(LPVOID argList)
    {
        //���� �̸� ĳ����
        const WCHAR* wFilename = (WCHAR*)argList;

        //OVERLAPPED ����ü ���� : �񵿱� ���� �۾��� �۾� ���� ������ �����ϴ� �Ϸ��� ���ؽ�Ʈ ������ �����ϴ� ����ü
        OVERLAPPED writeOV = { 0 };	//�ݵ�� �ʱ�ȭ �ؾ��Ѵ�.

        //�� ���Ͽ� ���� �ڵ��� �����Ѵ�.
        HANDLE hWrite = CreateFile(wFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);	//CREATE_ALWAYS�� ���� �߱⿡ �����ص� �����Ѵ�.

        LARGE_INTEGER writeBytes = { 0 };				//�� ����Ʈ ��
        DWORD dwOffset = MAX_NUMBER_OF_BYTE_TO_WRITE;

        //��� ���ҵǴ��� Ȯ��
        m_numberOfFragment = m_fileSize.QuadPart / MAX_NUMBER_OF_BYTE_TO_WRITE;
        if (m_fileSize.QuadPart % MAX_NUMBER_OF_BYTE_TO_WRITE) { m_numberOfFragment++; }

        //������ �ƴ϶�� ���� ���⸦ �����Ѵ�.
        if (hWrite != INVALID_HANDLE_VALUE)
        {
            while (true)
            {
                //�������� : �� ����Ʈ ���� ��ü ����Ʈ ���� ���ų� Ŭ ��(�̶��� ��������)
                if (writeBytes.QuadPart >= m_fileSize.QuadPart) { break; }

                //���� �۾��� ���� �뷮�� �����¿� �ش��ϴ� ����Ʈ ����ŭ�� ���� �� �ִ��� Ȯ���Ѵ�.
                if (m_fileSize.QuadPart - writeBytes.QuadPart < dwOffset) { dwOffset = m_fileSize.QuadPart - writeBytes.QuadPart; }

                //���� �۾��� �����Ѵ�. ���� �߻��� false�� ��ȯ�Ѵ�.
                if (!asyncWriteSub(hWrite, dwOffset, writeBytes, writeOV))
                {
                    CloseHandle(hWrite);
                    return 0;
                }

                SetEvent(m_hProgressPrint);
                m_curOPCount++;
            }

            // ���� - ���� ������̵� �ڵ��� �ݴ´�.
            CloseHandle(hWrite);
        }

        return writeBytes.LowPart;
    }

    bool FileIOManager::asyncReadSub(HANDLE handle, DWORD dwOffset, LARGE_INTEGER& readBytes, OVERLAPPED& readOV)
    {
        DWORD dwRead = 0;
        bool bPending = false;

        BOOL bRet = ReadFile(handle, &m_pfileBuf[readBytes.QuadPart], dwOffset, &dwRead, &readOV);		//�񵿱� �б� ����

        if (bRet == FALSE)
        {
            DWORD dwRet = GetLastError();
            if (dwRet != ERROR_IO_PENDING) { return false; }
            else { bPending = true; }
        }

        //ù �бⰡ ������ ��� �۾��� �Ϸ�� �� ���� �񵿱� �۾��� ���� ���¸� Ȯ���Ѵ�.
        while (bPending)
        {
            bRet = GetOverlappedResult(handle, &readOV, &dwRead, FALSE);	//������ �μ��� �۾��� �Ϸ� �� �� ���� ��ȯ�� �̷����� �����Ѵ�.

            if (bRet == TRUE) { bPending = false; }
        }

        //���� ����Ʈ ���� �����Ѵ�.
        readBytes.QuadPart += dwRead;

        //���� ����Ʈ ����ŭ �������� �����Ѵ�.
        readOV.Offset = readBytes.LowPart;
        readOV.OffsetHigh = readBytes.HighPart;

        return true;
    }

    bool FileIOManager::asyncWriteSub(HANDLE handle, DWORD dwOffset, LARGE_INTEGER& writeBytes, OVERLAPPED& writeOV)
    {
        DWORD dwWrite = 0;
        bool bPending = false;

        BOOL bRet = WriteFile(handle, &m_pfileBuf[writeBytes.QuadPart], dwOffset, &dwWrite, &writeOV);		//�񵿱� �б� ����

        if (bRet == FALSE)
        {
            DWORD dwRet = GetLastError();
            if (dwRet != ERROR_IO_PENDING) { return false; }
            else { bPending = true; }
        }

        //ù ���Ⱑ ������ ��� �۾��� �Ϸ�� �� ���� �񵿱� �۾��� ���� ���¸� Ȯ���Ѵ�.
        while (bPending)
        {
            bRet = GetOverlappedResult(handle, &writeOV, &dwWrite, FALSE);	//������ �μ��� �۾��� �Ϸ� �� �� ���� ��ȯ�� �̷����� �����Ѵ�.

            if (bRet == TRUE) { bPending = false; }
        }

        //�� ����Ʈ ���� �����Ѵ�.
        writeBytes.QuadPart += dwWrite;

        //�� ����Ʈ ����ŭ �������� �����Ѵ�.
        writeOV.Offset = writeBytes.LowPart;
        writeOV.OffsetHigh = writeBytes.HighPart;
        return true;
    }


    bool loadDir(std::wstring wszDirName,
        std::vector<std::wstring>* pWszInExtArr,
        std::vector<std::wstring>* pWszOutPathArr,
        std::vector<std::string>* pSzOutNameArr)
    {
        if (pWszOutPathArr && pSzOutNameArr)
        {
            _wfinddata_t fileData;

            std::wstring searchPath = wszDirName + L"*.*";
            intptr_t handle = _wfindfirst(searchPath.c_str(), &fileData);

            if (handle != -1)
            {
                do
                {
                    if ((fileData.attrib & _A_SUBDIR) && (fileData.name[0] != L'.'))
                    {
                        loadDir(wszDirName + fileData.name + L'\\', pWszInExtArr, pWszOutPathArr, pSzOutNameArr);
                    }
                    else if (fileData.name[0] != L'.')
                    {
                        std::wstring filePath = wszDirName + fileData.name;
                        std::wstring fileName;

                        if (pWszInExtArr && pWszInExtArr->size())
                        {
                            if (getSplitName(filePath, pWszInExtArr, fileName))
                            {
                                pWszOutPathArr->push_back(filePath);
                                pSzOutNameArr->push_back(atl_W2M(fileName));
                            }
                        }
                        else
                        {
                            pWszOutPathArr->push_back(filePath);
                            pSzOutNameArr->push_back(atl_W2M(fileName));
                        }
                    }
                } while (!_wfindnext(handle, &fileData));

                return true;
            }
        }

        return false;
    }

    bool loadDir(std::wstring wszDirName,
        std::vector<std::wstring>* pWszInExtArr,
        std::vector<std::wstring>* pWszOutPathArr,
        std::vector<std::wstring>* pWszOutNameArr)
    {
        if (pWszOutPathArr && pWszOutNameArr)
        {
            _wfinddata_t fileData;

            std::wstring searchPath = wszDirName + L"*.*";
            intptr_t handle = _wfindfirst(searchPath.c_str(), &fileData);

            if (handle != -1)
            {
                do
                {
                    if ((fileData.attrib & _A_SUBDIR) && (fileData.name[0] != L'.'))
                    {
                        loadDir(wszDirName + fileData.name + L'\\', pWszInExtArr, pWszOutPathArr, pWszOutNameArr);
                    }
                    else if (fileData.name[0] != L'.')
                    {
                        std::wstring filePath = wszDirName + fileData.name;
                        std::wstring fileName;

                        if (pWszInExtArr && pWszInExtArr->size())
                        {
                            if (getSplitName(filePath, pWszInExtArr, fileName))
                            {
                                pWszOutPathArr->push_back(filePath);
                                pWszOutNameArr->push_back(fileName);
                            }
                        }
                        else
                        {
                            pWszOutPathArr->push_back(filePath);
                            pWszOutNameArr->push_back(fileName);
                        }
                    }
                } while (!_wfindnext(handle, &fileData));

                return true;
            }
        }

        return false;
    }

    bool isValidName(std::string szName)
    {
        for (auto it : checkCharSet)
        {
            size_t ret = szName.find(it);
            if (ret != std::string::npos) { return false; }
        }

        return true;
    }

    bool isValidName(std::wstring wszName)
    {
        for (auto it : checkCharSet)
        {
            size_t ret = wszName.find(it);
            if (ret != std::wstring::npos) { return false; }
        }

        return true;
    }

    bool getSplitName(std::wstring& wszFullPath, std::wstring& wszOutfileName)
    {
        std::wstring ret;

        wchar_t drive[MAX_PATH] = { 0 };
        wchar_t dir[MAX_PATH] = { 0 };
        wchar_t fileName[MAX_PATH] = { 0 };
        wchar_t extension[MAX_PATH] = { 0 };

        //���� ��� ����
        _wsplitpath_s(wszFullPath.c_str(), drive, dir, fileName, extension);

        wszOutfileName = fileName;
        wszOutfileName += extension;

        return true;
    }

    bool getSplitName(std::string& szFullPath, std::wstring& wszOutfileName)
    {
        std::wstring ret;

        wchar_t drive[MAX_PATH] = { 0 };
        wchar_t dir[MAX_PATH] = { 0 };
        wchar_t fileName[MAX_PATH] = { 0 };
        wchar_t extension[MAX_PATH] = { 0 };

        //���� ��� ����
        _wsplitpath_s(EFFECTUTIL::atl_M2W(szFullPath).c_str(), drive, dir, fileName, extension);

        wszOutfileName = fileName;
        wszOutfileName += extension;
        return true;
    }

    bool getSplitDir(std::wstring& wszFullPath, std::wstring& wszOutDirName, bool bContainFileName)
    {
        std::wstring ret;

        wchar_t drive[MAX_PATH] = { 0 };
        wchar_t dir[MAX_PATH] = { 0 };
        wchar_t fileName[MAX_PATH] = { 0 };
        wchar_t extension[MAX_PATH] = { 0 };

        //���� ��� ����
        _wsplitpath_s(wszFullPath.c_str(), drive, dir, fileName, extension);

        wszOutDirName = drive;
        wszOutDirName += dir;

        if (bContainFileName)
        {
            wszOutDirName += fileName;
        }

        return true;
    }

    bool getSplitDir(std::string& szFullPath, std::wstring& wszOutDirName, bool bContainFileName)
    {
        std::wstring ret;

        wchar_t drive[MAX_PATH] = { 0 };
        wchar_t dir[MAX_PATH] = { 0 };
        wchar_t fileName[MAX_PATH] = { 0 };
        wchar_t extension[MAX_PATH] = { 0 };

        //���� ��� ����
        _wsplitpath_s(EFFECTUTIL::atl_M2W(szFullPath).c_str(), drive, dir, fileName, extension);

        wszOutDirName = drive;
        wszOutDirName += dir;

        if (bContainFileName)
        {
            wszOutDirName += fileName;
        }

        return true;
    }

    bool getSplitName(std::wstring& wszFullPath, std::vector<std::wstring>* pWszInExtArr, std::wstring& wszOutfileName)
    {
        if (pWszInExtArr)
        {
            std::wstring ret;

            wchar_t drive[MAX_PATH] = { 0 };
            wchar_t dir[MAX_PATH] = { 0 };
            wchar_t fileName[MAX_PATH] = { 0 };
            wchar_t extension[MAX_PATH] = { 0 };

            //���� ��� ����
            _wsplitpath_s(wszFullPath.c_str(), drive, dir, fileName, extension);

            ret = extension;

            for (int i = 0; i < pWszInExtArr->size(); i++)
            {
                if ((*pWszInExtArr)[i] == ret)
                {
                    wszOutfileName = fileName + ret;
                    return true;
                }
            }
        }

        return false;
    }

    bool getSplitName(std::string& szFullPath, std::vector<std::wstring>* pWszInExtArr, std::wstring& wszOutfileName)
    {
        if (pWszInExtArr)
        {
            std::wstring ret;

            wchar_t drive[MAX_PATH] = { 0 };
            wchar_t dir[MAX_PATH] = { 0 };
            wchar_t fileName[MAX_PATH] = { 0 };
            wchar_t extension[MAX_PATH] = { 0 };

            //���� ��� ����
            _wsplitpath_s(EFFECTUTIL::atl_M2W(szFullPath).c_str(), drive, dir, fileName, extension);

            std::wstring ext = extension;
            if (ext == L".tga" || ext == L".TGA") { ext = L".dds"; }

            ret += ext;

            for (int i = 0; i < pWszInExtArr->size(); i++)
            {
                if ((*pWszInExtArr)[i] == ret)
                {
                    wszOutfileName = fileName + ret;
                    return true;
                }
            }
        }

        return false;
    }
}