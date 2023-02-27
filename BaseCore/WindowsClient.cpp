#include "WindowsClient.h"

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
	case WM_SIZE:
	{
		
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
		OutputDebugStringA("WanyCore::WindowUI::Failed Create Window.\n");
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
	wcex.lpszClassName = L"WindowUI"; // ������ ���� ����... hWnd = CreateWindow �� ���� �̸��� ���ƾ� ��.
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

bool WindowsClient::Create(HINSTANCE instance, const WCHAR* title, LONG width, LONG height)
{
	hInstance = instance;
	WORD rst = MyRegisterClass();
	if (!InitInstance(title, width, height))
	{
		OutputDebugStringA("WanyCore::Windows::Failed InitInstance.\n");
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