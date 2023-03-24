#include "WindowsClient.h"
#include "DXDevice.hpp"
#include "Input.hpp"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//case WM_COMMAND:
		//{
		//	int wmId = LOWORD(wParam);
		//	// �޴� ������ ���� �м��մϴ�:
		//	switch (wmId)
		//	{
		//	case IDM_ABOUT:
		//		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		//		break;
		//	case IDM_EXIT:
		//		DestroyWindow(hWnd);
		//		break;
		//	default:
		//		// Wany
		//		// DefWindowProc: �޼��� ���� ó�� �Ұ�, ��� ó�� �� �ִ� �Լ�.
		//		return DefWindowProc(hWnd, message, wParam, lParam);
		//	}
		//}
		//break;
		/*case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			EndPaint(hWnd, &ps);
		}
		break;*/

	case WM_MOUSEWHEEL:
	{
		//wParam ���� Word : ���� ȸ���Ǵ� �Ÿ��� WHEEL_DELTA����� ǥ���ȴ�. ����� ��, ������ �ڷ� ȸ��
		//	GET_WHEEL_DELTA_WPARAM���� �����Ѵ�.
		//wParam ���� Word : ���콺 �� �Է°� ���� �߻��� Ű�Է�. MK_��ũ�η� ���޵ȴ�.
		//	GET_KEYSTATE_WPARAM���� �����Ѵ�.
		//lParam ���� Word : ȭ�� ���� ����� �������� �ϴ� ���콺�� y��ǥ
		//	GET_Y_LPARAM���� �����Ѵ�. HIWORD�� ����ϸ� �ȵȴ�(�������� �� ���� �ִµ� DWORD�� ��ȯ�ϱ� ����)
		//lParam ���� Word : ȭ�� ���� ����� �������� �ϴ� ���콺�� x��ǥ
		//	GET_X_LPARAM���� �����Ѵ�. LOWORD�� ����ϸ� �ȵȴ�(�������� �� ���� �ִµ� DWORD�� ��ȯ�ϱ� ����)

		MOUSE_WHEEL_STATE* pWState = &(Input::GetInstance()->m_wheelState);

		pWState->bMoveWheel = true;
		pWState->iWheelDt			+= GET_WHEEL_DELTA_WPARAM(wParam);
		pWState->dwExtraKeyInput	= GET_KEYSTATE_WPARAM(wParam);
		pWState->pt.x				= EXTRACT_LPARAM_X(lParam);
		pWState->pt.y				= EXTRACT_LPARAM_Y(lParam);

		ScreenToClient(hWnd, &pWState->pt);

		return 0;
	}
	
	case WM_SIZE:
	{
		//�ּ�ȭ�� �ƴ� ��쿡�� DX���ҽ� ũ�� ������ �۾��� �����Ѵ�.
		if (wParam != SIZE_MINIMIZED)
		{
			Input::GetInstance()->m_bMinimized = false;

			if (DXDevice::g_hWnd != NULL)
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				DXDevice::Resize(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			}
		}
		else 
		{
			Input::GetInstance()->m_bMinimized = true;
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool WindowsClient::InitInstance(const WCHAR* title, LONG width, LONG height)
{
	// Wany
	// �ü���� �����(RegisterWindow) �����츦 �����Ѵ�.
	DWORD csStyle = WS_OVERLAPPEDWINDOW;
	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, csStyle, FALSE);

	int clientWidth = rc.right - rc.left;
	int clientHeight = rc.bottom - rc.top;
	int monitorWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int monitorHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	int x = static_cast<UINT>((monitorWidth - clientWidth) * 0.5f);
	int y = static_cast<UINT>((monitorHeight - clientHeight) * 0.5f);

	hWnd = CreateWindowExW(0, L"Windows", title, WS_OVERLAPPEDWINDOW, x, y, clientWidth, clientHeight, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		OutputDebugStringA("WindowUI::Failed Create Window.\n");
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW);
	//GetClientRect(hWnd, &clientRect);

	return true;
}

ATOM WindowsClient::MyRegisterClass()
{
	// Wany
	// �ü������ �ش� �����츦 ���� ��� ��û�ϴ� �Լ�.
	// WND: window, Class, EX: extension, W: wide(unicode)
	WNDCLASSEXW wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEXW));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	// Wany
	// lp: pointer, fn: function, Wnd: window, Proc: procedure, �԰� ����������. ���� �߿�
	// ������ �޼����� ���� �Լ��� ����.
	wcex.lpfnWndProc = WndProc; // ���� �Լ��� Static�� �Է� ����.
	//wcex.cbClsExtra = 0;
	//wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSTUDY0));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSTUDY0);
	wcex.lpszClassName = L"Windows"; // ������ ���� ����... hWnd = CreateWindow �� ���� �̸��� ���ƾ� ��.
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

bool WindowsClient::Create(HINSTANCE instance, const WCHAR* title, LONG width, LONG height)
{
	hInstance = instance;
	WORD rst = MyRegisterClass();
	if (!InitInstance(title, width, height))
	{
		OutputDebugStringA("Windows::Failed InitInstance.\n");
		return false;
	}
	return true;
}

HWND WindowsClient::GetHandle()
{
	return hWnd;
}

bool WindowsClient::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // �޼��� ����
			DispatchMessage(&msg); // �޼��� ���ν����� ����
		}
		else
		{
			return true;
		}
	}

	return false;
}