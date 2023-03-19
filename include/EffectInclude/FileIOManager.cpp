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

        //이벤트 핸들 생성
        m_hOPStart = CreateEvent(0, FALSE, FALSE, nullptr);		//자동 리셋 이벤트로 생성
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

        //스레드 생성부
        UINT idRead;

        //반환값인 uintptr_t는 스레드의 핸들처럼 사용된다.
        uintptr_t hReadThread = _beginthreadex(nullptr, 0, asyncReadProc, (LPVOID)wszFilePath.c_str(), 0, &idRead);
        if (!hReadThread) { CloseHandle(m_hOPStart), CloseHandle(m_hProgressPrint); return false; }

        while (true)
        {
            //이벤트 대기함수 : 타임아웃 시간과 그 시간 만큼 기다릴 이벤트 핸들을 설정한다. 
            //이벤트 핸들은 일종의 함수를 가리키는 핸들이며 이벤트 핸들이 가리키는 이벤트 또는 함수가 발생시킨 반환값에 따라 이 함수의 반환값이 달라진다.
            //반환값
                //정상적인 이벤트 결과 세트 : WAIT_OBJECT_0 => 0
                //타임아웃 : WAIT_TIMEOUT => 0x00000102L
                //실패 : WAIT_FAILED => 0xFFFFFFFF

            if (WaitForSingleObject(m_hProgressPrint, INFINITE) == WAIT_OBJECT_0)
            {
                //진행 상황의 백분율을 출력한다.
                float fProgress = (float)m_curOPCount * 100.0f / m_numberOfFragment;

                std::wstring progressStr = L"In Progress : ";
                progressStr += std::to_wstring(fProgress) + L"%\n";
                OutputDebugString(progressStr.c_str());

                if (m_numberOfFragment == m_curOPCount) { break; }
            }
        }

        //스레드 종료를 기다린다.
        WaitForSingleObject((HANDLE)hReadThread, INFINITE);

        //스레드가 종료되면 핸들을 닫는다.
        CloseHandle((HANDLE)hReadThread);
        CloseHandle(m_hOPStart);
        CloseHandle(m_hProgressPrint);

        memcpy(destLoadData, m_pfileBuf, m_fileSize.LowPart);

        //동적 할당한 파일 버퍼를 해제한다.
        delete[] m_pfileBuf;

        return bRet;
    }

    bool FileIOManager::saveFile(std::wstring wszFilePath, void* srcSaveData, UINT iBytes)
    {
        init();

        //이벤트 핸들 생성
        m_hOPStart = CreateEvent(0, TRUE, FALSE, nullptr);		//자동 리셋 이벤트로 생성
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

        //반환값인 uintptr_t는 스레드의 핸들처럼 사용된다.
        uintptr_t hWriteThread = _beginthreadex(nullptr, 0, asyncWriteProc, (LPVOID)wszFilePath.c_str(), 0, &idWrite);
        if (!hWriteThread) { CloseHandle(m_hOPStart), CloseHandle(m_hProgressPrint); return false; }

        while (true)
        {
            //이벤트 대기함수 : 타임아웃 시간과 그 시간 만큼 기다릴 이벤트 핸들을 설정한다. 
            //이벤트 핸들은 일종의 함수를 가리키는 핸들이며 이벤트 핸들이 가리키는 이벤트 또는 함수가 발생시킨 반환값에 따라 이 함수의 반환값이 달라진다.
            //반환값
                //정상적인 이벤트 결과 세트 : WAIT_OBJECT_0 => 0
                //타임아웃 : WAIT_TIMEOUT => 0x00000102L
                //실패 : WAIT_FAILED => 0xFFFFFFFF
            if (WaitForSingleObject(m_hProgressPrint, INFINITE) == WAIT_OBJECT_0)
            {
                //진행 상황의 백분율을 출력한다.
                float fProgress = (float)m_curOPCount * 100.0f / m_numberOfFragment;

                std::wstring progressStr = L"In Progress : ";
                progressStr += std::to_wstring(fProgress) + L"%\n";
                OutputDebugString(progressStr.c_str());

                if (m_numberOfFragment == m_curOPCount) { break; }
            }
        }

        //스레드 종료를 기다린다.
        WaitForSingleObject((HANDLE)hWriteThread, INFINITE);

        //스레드가 종료되면 핸들을 닫는다.
        CloseHandle((HANDLE)hWriteThread);
        CloseHandle(m_hOPStart);
        CloseHandle(m_hProgressPrint);

        //동적 할당한 파일 버퍼를 해제한다.
        delete[] m_pfileBuf;
        m_pfileBuf = nullptr;

        return true;
    }

    UINT FileIOManager::asyncReadProc(LPVOID argList)
    {
        //파일 이름 캐스팅
        const WCHAR* wFilename = (WCHAR*)argList;

        //OVERLAPPED 구조체 생성 : 비동기 파일 작업의 작업 진행 정보를 저장하는 일련의 컨텍스트 역할을 수행하는 구조체
        OVERLAPPED readOV = { 0 };	//반드시 초기화 해야한다.

        //읽을 파일에 대한 핸들을 생성한다. : 비동기 작업시 FILE_FLAG_OVERLAPPED와 더불어 OVERLAPPED구조체가 필요하다.
        HANDLE hRead = CreateFile(wFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

        LARGE_INTEGER readBytes = { 0 };				//읽은 바이트 수
        DWORD dwOffset = MAX_NUMBER_OF_BYTE_TO_READ;

        //오류가 아니라면 파일 읽기를 수행한다.
        if (hRead != INVALID_HANDLE_VALUE)
        {
            ::GetFileSizeEx(hRead, &m_fileSize);			//파일 크기 불러오기
            m_pfileBuf = new BYTE[m_fileSize.LowPart];		//버퍼 동적 생성

            //몇번 분할 되는지 확인
            m_numberOfFragment = m_fileSize.QuadPart / MAX_NUMBER_OF_BYTE_TO_READ;
            if (m_fileSize.QuadPart % MAX_NUMBER_OF_BYTE_TO_READ) { m_numberOfFragment++; }

            while (true)
            {
                //종료조건 : 읽은 바이트 수가 전체 바이트 수와 같거나 클 때(이때는 오류겠지)
                if (readBytes.QuadPart >= m_fileSize.QuadPart) { break; }

                //읽기 작업전 남은 용량이 오프셋에 해당하는 바이트 수만큼을 읽을 수 있는지 확인한다.
                if (m_fileSize.QuadPart - readBytes.QuadPart < dwOffset) { dwOffset = m_fileSize.QuadPart - readBytes.QuadPart; }

                //읽기 작업을 수행한다. 오류 발생시 false를 반환한다.
                if (!asyncReadSub(hRead, dwOffset, readBytes, readOV))
                {
                    CloseHandle(hRead);
                    return 0;
                }

                SetEvent(m_hProgressPrint);
                m_curOPCount++;
            }

            // 성공 - 실패 어느쪽이든 핸들을 닫는다.
            CloseHandle(hRead);
        }
        else
        {
            ResetEvent(m_hProgressPrint);
        }

        return readBytes.LowPart;									//읽은 크기 반환
    }

    UINT FileIOManager::asyncWriteProc(LPVOID argList)
    {
        //파일 이름 캐스팅
        const WCHAR* wFilename = (WCHAR*)argList;

        //OVERLAPPED 구조체 생성 : 비동기 파일 작업의 작업 진행 정보를 저장하는 일련의 컨텍스트 역할을 수행하는 구조체
        OVERLAPPED writeOV = { 0 };	//반드시 초기화 해야한다.

        //쓸 파일에 대한 핸들을 생성한다.
        HANDLE hWrite = CreateFile(wFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);	//CREATE_ALWAYS를 설정 했기에 실패해도 생성한다.

        LARGE_INTEGER writeBytes = { 0 };				//쓴 바이트 수
        DWORD dwOffset = MAX_NUMBER_OF_BYTE_TO_WRITE;

        //몇번 분할되는지 확인
        m_numberOfFragment = m_fileSize.QuadPart / MAX_NUMBER_OF_BYTE_TO_WRITE;
        if (m_fileSize.QuadPart % MAX_NUMBER_OF_BYTE_TO_WRITE) { m_numberOfFragment++; }

        //오류가 아니라면 파일 쓰기를 시작한다.
        if (hWrite != INVALID_HANDLE_VALUE)
        {
            while (true)
            {
                //종료조건 : 쓴 바이트 수가 전체 바이트 수와 같거나 클 때(이때는 오류겠지)
                if (writeBytes.QuadPart >= m_fileSize.QuadPart) { break; }

                //쓰기 작업전 남은 용량이 오프셋에 해당하는 바이트 수만큼을 읽을 수 있는지 확인한다.
                if (m_fileSize.QuadPart - writeBytes.QuadPart < dwOffset) { dwOffset = m_fileSize.QuadPart - writeBytes.QuadPart; }

                //복사 작업을 수행한다. 오류 발생시 false를 반환한다.
                if (!asyncWriteSub(hWrite, dwOffset, writeBytes, writeOV))
                {
                    CloseHandle(hWrite);
                    return 0;
                }

                SetEvent(m_hProgressPrint);
                m_curOPCount++;
            }

            // 성공 - 실패 어느쪽이든 핸들을 닫는다.
            CloseHandle(hWrite);
        }

        return writeBytes.LowPart;
    }

    bool FileIOManager::asyncReadSub(HANDLE handle, DWORD dwOffset, LARGE_INTEGER& readBytes, OVERLAPPED& readOV)
    {
        DWORD dwRead = 0;
        bool bPending = false;

        BOOL bRet = ReadFile(handle, &m_pfileBuf[readBytes.QuadPart], dwOffset, &dwRead, &readOV);		//비동기 읽기 수행

        if (bRet == FALSE)
        {
            DWORD dwRet = GetLastError();
            if (dwRet != ERROR_IO_PENDING) { return false; }
            else { bPending = true; }
        }

        //첫 읽기가 성공한 경우 작업이 완료될 때 까지 비동기 작업의 현재 상태를 확인한다.
        while (bPending)
        {
            bRet = GetOverlappedResult(handle, &readOV, &dwRead, FALSE);	//마지막 인수는 작업이 완료 될 때 까지 반환을 미룰지를 결정한다.

            if (bRet == TRUE) { bPending = false; }
        }

        //읽은 바이트 수를 누적한다.
        readBytes.QuadPart += dwRead;

        //읽은 바이트 수만큼 오프셋을 조정한다.
        readOV.Offset = readBytes.LowPart;
        readOV.OffsetHigh = readBytes.HighPart;

        return true;
    }

    bool FileIOManager::asyncWriteSub(HANDLE handle, DWORD dwOffset, LARGE_INTEGER& writeBytes, OVERLAPPED& writeOV)
    {
        DWORD dwWrite = 0;
        bool bPending = false;

        BOOL bRet = WriteFile(handle, &m_pfileBuf[writeBytes.QuadPart], dwOffset, &dwWrite, &writeOV);		//비동기 읽기 수행

        if (bRet == FALSE)
        {
            DWORD dwRet = GetLastError();
            if (dwRet != ERROR_IO_PENDING) { return false; }
            else { bPending = true; }
        }

        //첫 쓰기가 성공한 경우 작업이 완료될 때 까지 비동기 작업의 현재 상태를 확인한다.
        while (bPending)
        {
            bRet = GetOverlappedResult(handle, &writeOV, &dwWrite, FALSE);	//마지막 인수는 작업이 완료 될 때 까지 반환을 미룰지를 결정한다.

            if (bRet == TRUE) { bPending = false; }
        }

        //쓴 바이트 수를 누적한다.
        writeBytes.QuadPart += dwWrite;

        //쓴 바이트 수만큼 오프셋을 조정한다.
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

        //파일 경로 분해
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

        //파일 경로 분해
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

        //파일 경로 분해
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

        //파일 경로 분해
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

            //파일 경로 분해
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

            //파일 경로 분해
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