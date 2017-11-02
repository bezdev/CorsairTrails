// Trails.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include <stdlib.h> 
#include <tchar.h>  
#include <gdiplus.h>

#include "BezCorsairHelper.h"

#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;

#define RETURN_IF_FAILED(hr) do{ if (FAILED(hr)) { return -1; } }while(0)

// #define DEBUG

const unsigned int NUM_KEYS             = CLI_Last + 1;
const unsigned int KEY_HITS_IN_SPECTRUM = 150;
const int          MAX_LIT_TIME         = 1000 * 60 * 30 /*min*/;
const BezEffect    ACTIVE_EFFECT        = BezEffect::HeatSpectrum;
const LitColor     START_SPECTRUM       = { 0, 191, 255 };
const LitColor     END_SPECTRUM         = { 255, 0, 255 };

HHOOK _hook;
HWND g_hWnd;

int g_totalKeyHits;

struct CorsairKey
{
    bool isLit;
    bool isDown;
    unsigned int hitCount;
    std::chrono::high_resolution_clock::time_point litTime;
    LitColor litColor;

    void Reset()
    {
        isLit = false;
        isDown = false;
        hitCount = 0;
        litColor = { 0 };
    }
};

CorsairKey _corsairKeys[NUM_KEYS] = { 0 };

std::vector<std::pair<int, int>> _keyHistory;
int g_historySize = 10;

bool SortByKeyCount(std::pair<std::wstring, CorsairKey> k1, std::pair<std::wstring, CorsairKey> k2)
{
    if (k1.second.hitCount == k2.second.hitCount)
    {
        return (k1.first < k2.first);
    }

    return (k1.second.hitCount < k2.second.hitCount);
}

void handleNumPad(DWORD vkey)
{
    int size;
    CorsairLedColor* ledColors;
    GetLedsForNumber(vkey - VK_NUMPAD0, { 255, 0, 0 }, &ledColors, &size);

    CorsairSetLedsColorsAsync(size, ledColors, nullptr, nullptr);
    free(ledColors);
}

LRESULT __stdcall OnKeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= HC_ACTION)
    {
        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
        {
            KBDLLHOOKSTRUCT kbd = *((KBDLLHOOKSTRUCT*)lParam);
            auto ledId = GetLedIdForKey(kbd.vkCode, kbd.flags);

            _corsairKeys[ledId].isDown = false;
        }
        else if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            KBDLLHOOKSTRUCT kbd = *((KBDLLHOOKSTRUCT*)lParam);
            auto ledId = GetLedIdForKey(kbd.vkCode, kbd.flags);

            if (!_corsairKeys[ledId].isDown)
            {
                // Light up corsair key
                if (ledId != CLI_Invalid)
                {
                    if (ACTIVE_EFFECT == BezEffect::Random)
                    {
                        _corsairKeys[ledId].litColor = GetRandomLitColor();
                    }
                    else if (ACTIVE_EFFECT == BezEffect::HeatSpectrum)
                    {
                        _corsairKeys[ledId].litColor = GetLitColorInSpectrum(START_SPECTRUM, END_SPECTRUM, 100.f * _corsairKeys[ledId].hitCount / KEY_HITS_IN_SPECTRUM);
                    }

#ifdef DEBUG
                    std::cout << litColor.r << "," << litColor.g << "," << litColor.b << std::endl;
#endif

                    auto ledColor = CorsairLedColor{ ledId, _corsairKeys[ledId].litColor.r, _corsairKeys[ledId].litColor.g, _corsairKeys[ledId].litColor.b };
                    CorsairSetLedsColorsAsync(1, &ledColor, nullptr, nullptr);

                    _corsairKeys[ledId].litTime = std::chrono::high_resolution_clock::now();
                    _corsairKeys[ledId].isLit = true;
                }
#ifdef DEBUG
                else
                {
                    std::cout << "CLI_Invalid" << std::endl;
                }
#endif

                // Update history
                if (_keyHistory.size() > 0 &&
                    _keyHistory.back().first == ledId)
                {
                    _keyHistory.back().second++;
                }
                else
                {
                    _keyHistory.push_back(std::make_pair<int, int>(static_cast<int>(ledId), 1));
                }
                if (_keyHistory.size() > g_historySize)
                {
                    _keyHistory.erase(_keyHistory.begin());
                }

                _corsairKeys[ledId].hitCount++;
                g_totalKeyHits++;
            }

            _corsairKeys[ledId].isDown = true;

            InvalidateRect(g_hWnd, NULL, TRUE);
        }
    }

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

HRESULT InitializeCorsair()
{
    HRESULT hr = S_OK;

    CorsairPerformProtocolHandshake();

    if (const auto error = CorsairGetLastError())
    {
        std::cerr << "Handshake failed: " << error << std::endl;
        return E_FAIL;
    }

    if (!CorsairGetDeviceCount())
    {
        std::cerr << "No devices found" << std::endl;
        return E_FAIL;
    }

    CorsairRequestControl(CAM_ExclusiveLightingControl);

    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, OnKeyboardEvent, NULL, 0)))
    {
        std::cerr << "Hook failed" << std::endl;
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

void DrawGraph(HDC hdc, std::vector<std::pair<std::wstring, CorsairKey>>& hitKeys)
{
    if (hitKeys.size() == 0)
    {
        return;
    }

    Graphics graphics(hdc);
    Pen pen(Color(255, 0, 0, 255));
    SolidBrush blackBrush(Color(255, 54, 117, 229));

    FontFamily  fontFamily(L"Arial");
    Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
    Font        tinyFont(&fontFamily, 10, FontStyleRegular, UnitPixel);
    StringFormat barValueFormat;
    barValueFormat.SetLineAlignment(StringAlignmentFar);
    barValueFormat.SetAlignment(StringAlignmentCenter);
    StringFormat barKeyFormat;
    barKeyFormat.SetLineAlignment(StringAlignmentNear);
    barKeyFormat.SetAlignment(StringAlignmentCenter);
    StringFormat historyFormat;
    historyFormat.SetLineAlignment(StringAlignmentFar);
    historyFormat.SetAlignment(StringAlignmentNear);
    StringFormat historyRepeatFormat;
    historyRepeatFormat.SetLineAlignment(StringAlignmentNear);
    historyRepeatFormat.SetAlignment(StringAlignmentCenter);

    static const int HEIGHT = 300;
    static const int WIDTH = 600;
    static const POINT POSITION = { 0, 50 };

    static const int NUM_BARS = 10;
    static const int BAR_WIDTH = 40;
     
    int actualNumBars = hitKeys.size() > NUM_BARS ? NUM_BARS : static_cast<int>(hitKeys.size());
    float barStep = WIDTH / (actualNumBars + 1.0f);
    POINT barStart = { POSITION.x + barStep, POSITION.y + HEIGHT };

    {
        // Compute max height
        int maxHeight = 0;
        int i = 0;
        int currHitCount = 0;
        int mostHit = 0;
        std::vector<std::pair<std::wstring, CorsairKey>>::iterator it = hitKeys.end();
        while (it != hitKeys.begin() && i < actualNumBars)
        {
            it--;

            if (i == 0)
            {
                mostHit = (*it).second.hitCount;
            }

            bool isOtherBar = (i == actualNumBars - 1) && it != hitKeys.begin();

            if (!isOtherBar)
            {
                currHitCount += (*it).second.hitCount;
            }

            i++;
        }

        maxHeight = mostHit;
        if (mostHit < g_totalKeyHits - currHitCount)
        {
            maxHeight = g_totalKeyHits - currHitCount;
        }

        i = 0;
        currHitCount = 0;
        it = hitKeys.end();
        while (it != hitKeys.begin() && i < actualNumBars)
        {
            it--;

            SolidBrush colorBrush(Color(255, (*it).second.litColor.r, (*it).second.litColor.g, (*it).second.litColor.b));

            // If we overflow, a bar that combines all the hit counts from the other keys
            bool isOtherBar = (i == actualNumBars - 1) && it != hitKeys.begin();

            int hitCount = isOtherBar ? g_totalKeyHits - currHitCount : (*it).second.hitCount;
            currHitCount += hitCount;

            // Draw Bars
            int barHeight = hitCount / static_cast<float>(maxHeight) * HEIGHT;

            POINT barPos = { barStart.x - (BAR_WIDTH / 2.0) + (i * barStep), barStart.y - barHeight };
            Rect fillRect(barPos.x, barPos.y, BAR_WIDTH, barHeight);
            graphics.FillRectangle(&colorBrush, fillRect);

            // Draw value string
            wchar_t keyHitString[10];
            _itow(hitCount, keyHitString, 10);
            graphics.DrawString(
                keyHitString,
                wcslen(keyHitString),
                &font,
                PointF(barStart.x + (i * barStep), barStart.y - barHeight),
                &barValueFormat,
                &colorBrush);

            // Draw key string
            graphics.DrawString(
                isOtherBar ? L"Other" : (*it).first.c_str(),
                isOtherBar ? 5 : (*it).first.length(),
                &font,
                PointF(barStart.x + (i * barStep), barStart.y),
                &barKeyFormat,
                &blackBrush);

            i++;
        }
    }

    // Draw history
    {
        int i = 0;
        std::vector<std::pair<int, int>>::iterator it = _keyHistory.end();
        while (it != _keyHistory.begin())
        {
            it--;

            std::wstring key(KeyToString((*it).first));

            graphics.DrawString(
                key.c_str(),
                key.length(),
                &font,
                PointF(20, barStart.y + 100 + (i * 20)),
                &historyFormat,
                &blackBrush);

            if ((*it).second > 1)
            {
                wchar_t keyHitString[10];
                _itow((*it).second, keyHitString, 10);
                graphics.DrawString(
                    keyHitString,
                    wcslen(keyHitString),
                    &tinyFont,
                    PointF(20, barStart.y + 75 + (i * 20)),
                    &historyRepeatFormat,
                    &blackBrush);
            }

            i++;
        }
    }

    graphics.Flush(FlushIntention::FlushIntentionSync);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    std::vector<std::pair<std::wstring, CorsairKey>> hitKeys;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        
        // Sort key counts
        for (int i = 0; i < NUM_KEYS; i++)
        {
            if (_corsairKeys[i].hitCount > 0)
            {
                hitKeys.push_back(make_pair(std::wstring(KeyToString(i)), _corsairKeys[i]));
            }
        }

        std::sort(hitKeys.begin(), hitKeys.end(), SortByKeyCount);
        
        DrawGraph(hdc, hitKeys);

        hitKeys.clear();

        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        Cleanup();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    /*
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
    */

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR str, int nCmdShow)
{
    DWORD numInputs = 0;
    DWORD inputsRead = 0;

    if (FAILED(InitializeCorsair()))
    {
        Cleanup();
        return -1;
    }

    // Initialize GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("trails");
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Win32 Guided Tour"),
            NULL);

        return 1;
    }

    static TCHAR szWindowClass[] = _T("trails");
    static TCHAR szTitle[] = _T("Trails");

    g_hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        600, 800,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!g_hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("wtf"),
            NULL);

        return 1;
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

