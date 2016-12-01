// Trails.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "BezCorsairHelper.h"

// #define DEBUG

const unsigned int NUM_KEYS             = CLI_Last + 1;
const unsigned int KEY_HITS_IN_SPECTRUM = 150;
const int          MAX_LIT_TIME         = 1000 * 60 * 30 /*min*/;
const BezEffect    ACTIVE_EFFECT        = BezEffect::HeatSpectrum;
const LitColor     START_SPECTRUM       = { 0, 191, 255 };
const LitColor     END_SPECTRUM         = { 255, 0, 255 };

HHOOK _hook;

struct CorsairKey {
    bool isLit;
    unsigned int hitCount;
    std::chrono::high_resolution_clock::time_point litTime;

    void Reset()
    {
        isLit = false;
        hitCount = 0;
    }
};
CorsairKey _corsairKeys[NUM_KEYS] = { 0 };

void handleNumPad(DWORD vkey)
{
    int size;
    CorsairLedColor* ledColors;
    GetLedsForNumber(vkey - VK_NUMPAD0, { 255, 0, 0 }, &size, &ledColors);

    CorsairSetLedsColorsAsync(size, ledColors, nullptr, nullptr);
    free(ledColors);
}

LRESULT __stdcall onKeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= HC_ACTION)
    {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            KBDLLHOOKSTRUCT kbd = *((KBDLLHOOKSTRUCT*)lParam);

            if (kbd.vkCode >= VK_NUMPAD0 && kbd.vkCode <= VK_NUMPAD9)
            {
                handleNumPad(kbd.vkCode);
            }
            else
            {
                auto ledId = GetLedIdForKey(kbd.vkCode, kbd.flags);
                if (ledId != CLI_Invalid)
                {
                    LitColor litColor = { 0 };
                    if (ACTIVE_EFFECT == BezEffect::Random)
                    {
                        litColor = GetRandomLitColor();
                    }
                    else if (ACTIVE_EFFECT == BezEffect::HeatSpectrum)
                    {
                        litColor = GetLitColorInSpectrum(START_SPECTRUM, END_SPECTRUM, 100.f * _corsairKeys[ledId].hitCount / KEY_HITS_IN_SPECTRUM);
                    }

#ifdef DEBUG
                    std::cout << litColor.r << "," << litColor.g << "," << litColor.b << std::endl;
#endif

                    auto ledColor = CorsairLedColor{ ledId, litColor.r, litColor.g, litColor.b };
                    CorsairSetLedsColorsAsync(1, &ledColor, nullptr, nullptr);

                    _corsairKeys[ledId].litTime = std::chrono::high_resolution_clock::now();
                    _corsairKeys[ledId].isLit = true;
                    _corsairKeys[ledId].hitCount++;
                }
#ifdef DEBUG
                else
                {
                    std::cout << "CLI_Invalid" << std::endl;
                }
#endif
            }
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

    if (!CorsairGetDeviceCount()) {
        std::cerr << "No devices found" << std::endl;
        return E_FAIL;
    }

    CorsairRequestControl(CAM_ExclusiveLightingControl);

    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, onKeyboardEvent, NULL, 0)))
    {
        std::cout << "Hook failed" << std::endl;
        return E_FAIL;
    }

    srand(static_cast<unsigned int>(time(NULL)));
    
    auto winLockColor = CorsairLedColor{ CLK_WinLock, 255, 0, 0 };
    auto brightnessColor = CorsairLedColor{ CLK_Brightness, 255, 255, 255 };
    auto logoColor = CorsairLedColor{ CLK_Logo, 255, 0, 0 };

    CorsairSetLedsColorsAsync(1, &winLockColor, nullptr, nullptr);
    CorsairSetLedsColorsAsync(1, &brightnessColor, nullptr, nullptr);
    CorsairSetLedsColorsAsync(1, &logoColor, nullptr, nullptr);
    
    return S_OK;
}

void Cleanup()
{
    UnhookWindowsHookEx(_hook);
    CorsairReleaseControl(CAM_ExclusiveLightingControl);
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
			if (msg.message == WM_QUIT)
			{
				Cleanup();
				break;
			}

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for (int i = 0; i < NUM_KEYS; i++)
        {
            if (_corsairKeys[i].isLit)
            {
                std::chrono::duration<float, std::milli> elapsed = std::chrono::high_resolution_clock::now() - _corsairKeys[i].litTime;
                if (elapsed.count() > MAX_LIT_TIME)
                {
                    auto ledColor = CorsairLedColor{ static_cast<CorsairLedId>(i), 0, 0, 0 };
                    CorsairSetLedsColorsAsync(1, &ledColor, nullptr, nullptr);
                    _corsairKeys[i].Reset();
                }
            }
        }
    }

    return 0;
}

