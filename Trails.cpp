#include <windows.h>
#include <iostream>
#include <thread>
#include <string.h>
#include <stdlib.h> 
#include <tchar.h>  

#include "Util.h"
#include "CUESDK.h"
#include "InputManager.h"
#include "CorsairManager.h"
#include "GUI.h"
#include "Timer.h"

#define RETURN_IF_FAILED(hr) do { if (FAILED(hr)) { return -1; } } while(0)

static int WINDOW_WIDTH = 600;
static int WINDOW_HEIGHT = 800;
static TCHAR WINDOW_CLASS[] = _T("trails");
static TCHAR WINDOW_TITLE[] = _T("Trails");

// TOOD: factor out
const int MAX_LIT_TIME = 1000 * 60 * 30;  // 30 min

InputManager* g_InputManager;
GUI* g_GUI;

HHOOK g_KeyboardHook;
HHOOK g_MouseHook;
HWND g_HWND;
bool g_HasCorsairKeyboard;

LRESULT __stdcall OnKeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
	/*
	if (nCode >= HC_ACTION)
	{
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			//LOG(_T("keyboard hook down"));
			LOG(L"keyboard hook down");
			g_InputManager->OnKeyDown(wParam, lParam);
			InvalidateRect(g_HWND, NULL, TRUE);
		}
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		{
			g_InputManager->OnKeyUp(wParam, lParam);
		}
	}
	*/
    return CallNextHookEx(g_KeyboardHook, nCode, wParam, lParam);
}

LRESULT __stdcall OnMouseEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
	/*
	if (nCode >= HC_ACTION)
	{
		if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
		{
			//LOG(_T("mouse hook down"));
			LOG(L"mouse hook down");
			g_InputManager->OnMouseDown(wParam, lParam);
			InvalidateRect(g_HWND, NULL, TRUE);
		}
		else if (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP)
		{
			g_InputManager->OnMouseUp(wParam, lParam);
		}
	}
	*/

	return CallNextHookEx(g_MouseHook, nCode, wParam, lParam);
}

void Cleanup()
{
	UnhookWindowsHookEx(g_KeyboardHook);
	UnhookWindowsHookEx(g_MouseHook);
	CorsairReleaseControl(CAM_ExclusiveLightingControl);
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
	case WM_INPUT:
		g_InputManager->ProcessRawInput(wParam, lParam);
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
	MessageBox(hWnd, lpText, WINDOW_TITLE, NULL);
}

static bool HandleMessages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return true;
}

static void Run()
{
	Timer* timer = new Timer();
	auto s = scope_exit([timer]() {
		delete timer;
		});

	const int fps = 20;
	const float frameTime = 1000.f / fps;
	float delta = 0;

	// main loop
	bool isFirstMainLoop = true;
	while (HandleMessages())
	{
		if (isFirstMainLoop)
		{
			timer->Reset();
		}
		else
		{
			timer->Update();
		}

		delta += timer->GetDelta();

		if (delta > frameTime) {
			delta -= frameTime;

			InvalidateRect(g_HWND, NULL, TRUE);
		}

		//float f = timer->GetDelta() + 64.94f;

		/*
		LOG(L"delta: %f", f);
		LOG(L"delta: %d", (int)f);
		*/

		isFirstMainLoop = false;
	}
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
	
	/*
	if (!(g_KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, OnKeyboardEvent, NULL, 0)))
	{
		DisplayMessageBox(_T("Keyboard hook failed"));
		return E_FAIL;
	}
	if (!(g_MouseHook = SetWindowsHookEx(WH_MOUSE_LL, OnMouseEvent, NULL, 0)))
	{
		DisplayMessageBox(_T("Mouse hook failed"));
		return E_FAIL;
	}
	*/

	RAWINPUTDEVICE rid[3];
	rid[0] = { 0x01, 0x02, RIDEV_NOLEGACY | RIDEV_INPUTSINK, g_HWND }; // mouse
	rid[1] = { 0x01, 0x06, RIDEV_NOLEGACY | RIDEV_INPUTSINK, g_HWND }; // keyboard
	//rid[2] = { 0x01, 0x05, 0, g_HWND }; // controller
	if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0]))) {
		DisplayMessageBox(_T("Register RawInput failed"));
		return E_FAIL;
	}

    ShowWindow(g_HWND, nShowCmd);
	UpdateWindow(g_HWND);

	Run();
	
	return 0;
}
