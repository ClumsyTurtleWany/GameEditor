#include "Input.hpp"


void Input::setWndHandle(HWND _hWnd)
{
	hWnd = _hWnd;
}

bool Input::isPressedAnyKey()
{
	for (auto it : dwKeyState)
	{
		if ((it == KeyState::Down) || (it == KeyState::Hold))
		{
			return true;
		}
	}

	return false;
}

KeyState Input::getKey(DWORD _key)
{
	return dwKeyState[_key];
}

bool Input::initialize()
{
	ZeroMemory(&dwKeyState, sizeof(DWORD) * KEY_COUNT);
	GetCursorPos(&m_ptPos); // ȭ�� ��ǥ�� �� ���콺 Ŀ�� ��ġ
	ScreenToClient(hWnd, &m_ptPos); // Ŭ���̾�Ʈ ��ǥ�� ����.
	m_ptPrevPos = m_ptPos;
	
	return true;
}

bool Input::frame()
{
	GetCursorPos(&m_ptPos); // ȭ�� ��ǥ�� �� ���콺 Ŀ�� ��ġ
	ScreenToClient(hWnd, &m_ptPos); // Ŭ���̾�Ʈ ��ǥ�� ����.
	m_ptOffset.x = m_ptPos.x - m_ptPrevPos.x;
	m_ptOffset.y = m_ptPos.y - m_ptPrevPos.y;

	for (int key = 0; key < KEY_COUNT; key++)
	{
		SHORT state = GetAsyncKeyState(key); // �񵿱� Ű ����
		if (state & 0x8000) // 1000 0000 0000 0000 �ֻ��� ��Ʈ�� �������� Ȯ��
		{
			if ((dwKeyState[key] == KeyState::Free) || (dwKeyState[key] == KeyState::Up))
			{
				dwKeyState[key] = KeyState::Down;
			}
			else
			{
				dwKeyState[key] = KeyState::Hold;
			}
		}
		else
		{
			if ((dwKeyState[key] == KeyState::Down) || (dwKeyState[key] == KeyState::Hold))
			{
				dwKeyState[key] = KeyState::Up;
			}
			else
			{
				dwKeyState[key] = KeyState::Free;
			}
		}
	}

	m_ptPrevPos = m_ptPos;
	return true;
}

bool Input::render()
{
	//std::wstring mousePos = "Mouse Cursor: " + std::to_wstring(m_ptPos.x) + ", " + std::to_wstring(m_ptPos.y) + "\n";
	//OutputDebugStringA(mousePos.c_str());
	return true;
}

bool Input::release()
{

	return true;
}