// Trails.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <windows.h>
#include <iostream>
#include <thread>
#include <string.h>
#include <stdlib.h> 
#include <tchar.h>  
#include <gdiplus.h>

#include "InputManager.h"
#include "CorsairManager.h"
#include "BezCorsairHelper.h"
#include "GUI.h"

#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;

#define DEBUG

#define RETURN_IF_FAILED(hr) do { if (FAILED(hr)) { return -1; } } while(0)

#ifdef DEBUG
#define DEBUG_LOG(string) do { OutputDebugStringW(string "\n"); } while(0)
#else
#define DEBUG_LOG(string)
#endif

static int WINDOW_WIDTH = 600;
static int WINDOW_HEIGHT = 800;
static TCHAR WINDOW_CLASS[] = _T("trails");
static TCHAR WINDOW_TITLE[] = _T("Trails");

// TOOD: factor out
const int MAX_LIT_TIME = 1000 * 60 * 30;  // 30 min

InputManager* g_InputManager;
GUI* g_GUI;

HHOOK g_Hook;
HWND g_HWND;
bool g_HasCorsairKeyboard;

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
			g_InputManager->OnKeyUp(wParam, lParam);
		}
		else if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			g_InputManager->OnKeyDown(wParam, lParam);
			InvalidateRect(g_HWND, NULL, TRUE);
		}
	}

    return CallNextHookEx(g_Hook, nCode, wParam, lParam);
}

void Cleanup()
{
    UnhookWindowsHookEx(g_Hook);
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

		g_GUI->DrawStats(hdc, g_InputManager->GetKeys(), g_InputManager->GetTotalKeyHits(), g_InputManager->GetKeyHistory());

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

static void DisplayMessageBox(LPCTSTR lpText, HWND hWnd = NULL) {
	MessageBox(
		hWnd,
		lpText,
		WINDOW_TITLE,
		NULL);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	auto corsairManager = new CorsairManager();
	g_HasCorsairKeyboard = SUCCEEDED(corsairManager->Initialize());

	g_InputManager = new InputManager();
	g_GUI = new GUI();

	if (!(g_Hook = SetWindowsHookEx(WH_KEYBOARD_LL, OnKeyboardEvent, NULL, 0)))
	{
		DisplayMessageBox(_T("Hook failed"));
		return E_FAIL;
	}

    // Initialize GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
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
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = WINDOW_CLASS;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wcex))
    {
		DisplayMessageBox(_T("Call to RegisterClassEx failed"));
        return E_FAIL;
    }

    g_HWND = CreateWindow(
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH,
		WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!g_HWND)
    {
		DisplayMessageBox(_T("Call to CreateWindow failed"));
		return E_FAIL;
    }

    ShowWindow(g_HWND, nShowCmd);
    UpdateWindow(g_HWND);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
