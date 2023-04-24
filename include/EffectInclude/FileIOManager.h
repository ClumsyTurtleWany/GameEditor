#pragma once
#include "EffectDefine.h"
#include <io.h>
#include <process.h>

namespace EFFECTUTIL
{
	static const char checkCharSet[] = { '\\', '/', '?', '%', '*', '|', '\"', ':', '<', '>', '.', '\'' };
	static const char checkWcharSet[] = { L'\\', L'/', L'?', L'%', L'*', L'|', L'\"', L':', L'<', L'>', L'.', L'\'' };

	#define MAX_NUMBER_OF_BYTE_TO_READ			1024
	#define MAX_NUMBER_OF_BYTE_TO_WRITE			1024

	class FileIOManager
	{
	private:
		static BYTE* m_pfileBuf;				//파일 버퍼
		static LARGE_INTEGER		m_fileSize;
		static DWORD				m_numberOfFragment;
		static DWORD				m_curOPCount;

		//이벤트 핸들
		static HANDLE				m_hOPStart;
		static HANDLE				m_hProgressPrint;

		FileIOManager();
		~FileIOManager();
	public:
		FileIOManager(const FileIOManager& other) = delete;
		FileIOManager& operator=(const FileIOManager& other) = delete;

	public:
		bool init();
		bool release();

		bool loadFile(std::wstring wszFilePath, void* destLoadData);
		bool saveFile(std::wstring wszFilePath, void* srcSaveData, UINT iBytes);

		static UINT WINAPI asyncReadProc(LPVOID argList);
		static UINT WINAPI asyncWriteProc(LPVOID argList);

		static bool asyncReadSub(HANDLE handle, DWORD dwOffset, LARGE_INTEGER& readBytes, OVERLAPPED& readOV);
		static bool asyncWriteSub(HANDLE handle, DWORD dwOffset, LARGE_INTEGER& writeBytes, OVERLAPPED& writeOV);

		static FileIOManager& getInstance()
		{
			static FileIOManager singleInst;
			return singleInst;
		}
	};

	#define FILEIO_MGR FileIOManager::getInstance()

	bool getSplitName(std::string& szFullPath, std::wstring& wszOutfileName);
	bool getSplitName(std::wstring& wszFullPath, std::wstring& wszOutfileName);

	bool getSplitDir(std::string& szFullPath, std::wstring& wszOutDirName, bool bContainFileName = false);
	bool getSplitDir(std::wstring& wszFullPath, std::wstring& wszOutDirName, bool bContainFileName = false);

	bool getSplitName(std::string& szFullPath, std::vector<std::wstring>* pWszInExtArr, std::wstring& szOutfileName);
	bool getSplitName(std::wstring& wszFullPath, std::vector<std::wstring>* pWszInExtArr, std::wstring& szOutfileName);

	bool loadDir(std::wstring wszDirName,
		std::vector<std::wstring>* pWszInExtArr,
		std::vector<std::wstring>* pWszOutPathArr,
		std::vector<std::string>* pSzOutNameArr);

	bool loadDir(std::wstring wszDirName,
		std::vector<std::wstring>* pWszInExtArr,
		std::vector<std::wstring>* pWszOutPathArr,
		std::vector<std::wstring>* pWszOutNameArr);

	bool isValidName(std::string szName);
	bool isValidName(std::wstring wszName);
}