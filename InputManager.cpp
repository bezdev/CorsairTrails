#include "InputManager.h"

InputManager::InputManager()
{
}

void InputManager::OnKeyDown(unsigned short vkey)
{
	auto ledId = GetLedIdForKey(vkey, 0);
	LOG(L"down: %d:%s", ledId, KeyToString(ledId));

	if (!Keys[ledId].IsDown)
	{
		// Light up corsair key
		if (ledId != CLI_Invalid)
		{
			if (ACTIVE_EFFECT == BezEffect::Random)
			{
				Keys[ledId].Color = GetRandomColor();
			}
			else if (ACTIVE_EFFECT == BezEffect::HeatSpectrum)
			{
				Keys[ledId].Color = GetColorInSpectrum(START_SPECTRUM, END_SPECTRUM, 100.f * Keys[ledId].HitCount / KEY_HITS_IN_SPECTRUM);
			}

			auto ledColor = CorsairLedColor{ ledId, Keys[ledId].Color.r, Keys[ledId].Color.g, Keys[ledId].Color.b };
			CorsairSetLedsColorsAsync(1, &ledColor, nullptr, nullptr);

			Keys[ledId].LitTime = std::chrono::high_resolution_clock::now();
			Keys[ledId].IsLit = true;
		}
		else
		{
			LOG(L"CLI_Invalid");
		}

		UpdateHistory(ledId);

		Keys[ledId].HitCount++;
		TotalKeyHits++;
	}

	Keys[ledId].IsDown = true;
}

void InputManager::OnKeyUp(unsigned short vkey)
{
	auto ledId = GetLedIdForKey(vkey, 0);

	Keys[ledId].IsDown = false;
}

void InputManager::OnMouseDown(WPARAM wParam, LPARAM lParam)
{
}

void InputManager::OnMouseUp(WPARAM wParam, LPARAM lParam)
{
}

void InputManager::ProcessRawInput(WPARAM wParam, LPARAM lParam)
{
	UINT dwSize = 0;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if (lpb == NULL)
	{
		return;
	}

	auto s = scope_exit([lpb]() {
		delete[] lpb;
	});

	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
	{
		return;
	}

	RAWINPUT* raw = (RAWINPUT*)lpb;
	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0)
		{
			if (!MouseButtons[BezMouseButton::LEFT].IsDown) {
				MouseButtons[BezMouseButton::LEFT].HitCount++;
				Keys[LMB_ID].HitCount++;
				Keys[LMB_ID].Color = GetColorInSpectrum(START_SPECTRUM, END_SPECTRUM, 100.f * Keys[LMB_ID].HitCount / KEY_HITS_IN_SPECTRUM);
				UpdateHistory(LMB_ID);
				//LOG_DEBUG(_T("LMB down"));
			}

			MouseButtons[BezMouseButton::LEFT].IsDown = true;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) != 0)
		{
			MouseButtons[BezMouseButton::LEFT].IsDown = false;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0)
		{
			if (!MouseButtons[BezMouseButton::MIDDLE].IsDown) {
				MouseButtons[BezMouseButton::MIDDLE].HitCount++;
			}

			MouseButtons[BezMouseButton::MIDDLE].IsDown = true;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0)
		{
			MouseButtons[BezMouseButton::MIDDLE].IsDown = false;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0)
		{
			if (!MouseButtons[BezMouseButton::RIGHT].IsDown) {
				MouseButtons[BezMouseButton::RIGHT].HitCount++;
				Keys[RMB_ID].HitCount++;
				Keys[RMB_ID].Color = GetColorInSpectrum(START_SPECTRUM, END_SPECTRUM, 100.f * Keys[RMB_ID].HitCount / KEY_HITS_IN_SPECTRUM);
				UpdateHistory(RMB_ID);
			}

			MouseButtons[BezMouseButton::RIGHT].IsDown = true;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) != 0)
		{
			MouseButtons[BezMouseButton::RIGHT].IsDown = false;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) != 0)
		{
			if (!MouseButtons[BezMouseButton::M4].IsDown) {
				MouseButtons[BezMouseButton::M4].HitCount++;
			}

			MouseButtons[BezMouseButton::M4].IsDown = true;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) != 0)
		{
			MouseButtons[BezMouseButton::M4].IsDown = false;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) != 0)
		{
			if (!MouseButtons[BezMouseButton::M5].IsDown) {
				MouseButtons[BezMouseButton::M5].HitCount++;
			}

			MouseButtons[BezMouseButton::M5].IsDown = true;
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) != 0)
		{
			MouseButtons[BezMouseButton::M5].IsDown = false;
		}
	}
	else if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		LOG(L"RIM_TYPEKEYBOARD");
		LOG(L"flags: %d - vkey: %d", raw->data.keyboard.Flags, raw->data.keyboard.VKey);
		LOG(L"char: %c", static_cast<char>(raw->data.keyboard.VKey));
		if (raw->data.keyboard.Flags == 0)
		{
			OnKeyDown(raw->data.keyboard.VKey);
		}
		else if (raw->data.keyboard.Flags == 1)
		{
			OnKeyUp(raw->data.keyboard.VKey);
		}
		LOG(L"---");
	}
	else if (raw->header.dwType == RIM_TYPEHID)
	{
		LOG(_T("controller"));
	}
}

void InputManager::UpdateHistory(int id)
{
	// Update history
	if (KeyHistory.size() > 0 &&
		KeyHistory.back().first == id)
	{
		KeyHistory.back().second++;
	}
	else
	{
		std::pair<int,int> p { id, 1 };
		KeyHistory.push_back(p);
	}

	if (KeyHistory.size() > HISTORY_SIZE)
	{
		KeyHistory.erase(KeyHistory.begin());
	}

}

BezKey* InputManager::GetKeys()
{
	return Keys;
}

int InputManager::GetTotalKeyHits()
{
	int mouseHits = MouseButtons[BezMouseButton::LEFT].HitCount + MouseButtons[BezMouseButton::RIGHT].HitCount;
	return TotalKeyHits + mouseHits;
}

/*
void PrintKeyHitCounts(HDC hdc, std::vector<std::pair<std::wstring, int>>& keyHitCounts)
{
	int i = 0;
	std::vector<std::pair<std::wstring, int>>::iterator it = keyHitCounts.end();
	while (it != keyHitCounts.begin())
	{
		it--;

		wchar_t buffer[50];
		int n = wsprintf(buffer, L"%s: %d", (*it).first.c_str(), (*it).second);
		TextOut(hdc, 5, 5 + (i * 20), buffer, n);

		i++;
	}
}

void handleNumPad(DWORD vkey)
{
	int size;
	CorsairLedColor* ledColors;
	GetLedsForNumber(vkey - VK_NUMPAD0, { 255, 0, 0 }, &ledColors, &size);

	CorsairSetLedsColorsAsync(size, ledColors, nullptr, nullptr);
	free(ledColors);
}
*/
