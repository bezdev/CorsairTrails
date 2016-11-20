// Trails.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <Windows.h>
#include <iostream>
#include <chrono>

#include "helper.h"

//#define DEBUG

#define NUM_KEYS (CLI_Last + 1)
#define MAX_LIT_TIME 1000 * 60 * 15 // 15 min

HHOOK _hook;

struct CorsairKey {
	bool isLit;
	std::chrono::high_resolution_clock::time_point time;
};
CorsairKey _corsairKeys[NUM_KEYS] = { 0 };

LRESULT __stdcall onKeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= HC_ACTION)
	{
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			KBDLLHOOKSTRUCT kbd = *((KBDLLHOOKSTRUCT*)lParam);

#ifdef DEBUG
			std::cout << kbd.vkCode << std::endl;
			std::cout << kbd.flags << std::endl;
#endif

			auto ledId = GetLedIdForKey(kbd.vkCode, kbd.flags);
			if (ledId != CLI_Invalid)
			{
				auto ledColor = CorsairLedColor{ ledId, rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1 };
				CorsairSetLedsColorsAsync(1, &ledColor, nullptr, nullptr);
				_corsairKeys[ledId].isLit = true;
				_corsairKeys[ledId].time = std::chrono::high_resolution_clock::now();
			}

#ifdef DEBUG
			else
			{
				std::cout << "CLI_Invalid" << std::endl;
			}
#endif
		}
	}

	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

HRESULT Initialize()
{
	HRESULT hr = S_OK;

	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << error << std::endl;
		return E_FAIL;
	}

	CorsairRequestControl(CAM_ExclusiveLightingControl);

	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, onKeyboardEvent, NULL, 0)))
	{
		std::cout << "Hook failed" << std::endl;
		return E_FAIL;
	}

	srand(time(NULL));
	
	auto winLockColor = CorsairLedColor{ CLK_WinLock, 255, 0, 0 };
	auto brightnessColor = CorsairLedColor{ CLK_Brightness, 255, 255, 255 };

	CorsairSetLedsColorsAsync(1, &winLockColor, nullptr, nullptr);
	CorsairSetLedsColorsAsync(1, &brightnessColor, nullptr, nullptr);

	return S_OK;
}

void Cleanup()
{
	UnhookWindowsHookEx(_hook);
}

int main()
{
	if (FAILED(Initialize()))
	{
		Cleanup();
		return -1;
	}

	MSG msg;
	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			Cleanup();
			break;
		}

		for (int i = 0; i < NUM_KEYS; i++)
		{
			if (_corsairKeys[i].isLit)
			{
				std::chrono::duration<float, std::milli> elapsed = std::chrono::high_resolution_clock::now() - _corsairKeys[i].time;
				if (elapsed.count() > MAX_LIT_TIME)
				{
					auto ledColor = CorsairLedColor{ static_cast<CorsairLedId>(i), 0, 0, 0 };
					CorsairSetLedsColorsAsync(1, &ledColor, nullptr, nullptr);
					_corsairKeys[i].isLit = false;
				}
			}
		}
	}

    return 0;
}

