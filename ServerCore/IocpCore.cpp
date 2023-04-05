#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore GIocpCore;

/*--------------
	IocpCore
---------------*/

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

//bool IocpCore::Register(IocpObject* iocpObject)
bool IocpCore::Register(IocpObjectRef iocpObject)
{
	//return ::CreateIoCompletionPort(
	//	iocpObject->GetHandle(), _iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObject), 0);
	return ::CreateIoCompletionPort(
		iocpObject->GetHandle(), _iocpHandle, /*Key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	//IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;
	//if (::GetQueuedCompletionStatus(
	//	_iocpHandle, OUT & numOfBytes, OUT reinterp ret_cast<PULONG_PTR>(&iocpObject), 
	//	OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	//{
	//	//iocpObject(key), iocpEvent(overlapped) 성공적으로 복원
	//
	//	iocpObject->Dispatch(iocpEvent, numOfBytes);
	//}
	//else
	//{
	//	int32 errorCode = ::WSAGetLastError();
	//	switch (errorCode)
	//	{
	//	case WAIT_TIMEOUT:
	//		return false;
	//	default:
	//		//TODO : 로그 찍기
	//		iocpObject->Dispatch(iocpEvent, numOfBytes);
	//		break;
	//	}
	//}

	if (::GetQueuedCompletionStatus(
		_iocpHandle, OUT & numOfBytes, OUT &key,
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		//iocpObject(key), iocpEvent(overlapped) 성공적으로 복원
		IocpObjectRef iocpObject = iocpEvent->_owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
		forReturnErrorCode = 0;
	}
	else
	{
		int32 errorCode = ::WSAGetLastError();
		switch (errorCode)
		{
		case WAIT_TIMEOUT:
			return false;
		case ERROR_OPERATION_ABORTED:
			{
				forReturnErrorCode = errorCode;
				//if(auto acEvent = static_cast<AcceptEvent*>(iocpEvent))
			}break;
		case ERROR_CONNECTION_REFUSED:
			{
				forReturnErrorCode = errorCode;
				
			}break;
		default:
			//TODO : 로그 찍기
			IocpObjectRef iocpObject = iocpEvent->_owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}
	return true;
}
