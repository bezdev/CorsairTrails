#include <windows.h>
#include <iostream>
#include <thread>
#include <string.h>
#include <stdlib.h> 
#include <tchar.h>  

#include "Util.h"
#include "InputManager.h"
#include "CorsairManager.h"
#include "Renderer.h"
#include "Timer.h"

static int WINDOW_WIDTH = 600;
static int WINDOW_HEIGHT = 800;
static TCHAR WINDOW_CLASS[] = _T("trails");
static TCHAR WINDOW_TITLE[] = _T("Trails");

HWND g_HWND;

void Cleanup()
{
	CorsairReleaseControl(CAM_ExclusiveLightingControl);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_ERASEBKGND:
		return TRUE;
    case WM_PAINT:
		Renderer::GetInstance()->Render();
        break;
	case WM_INPUT:
		InputManager::GetInstance()->ProcessRawInput(wParam, lParam);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        Cleanup();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return TRUE;
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

	const int fps = 60;
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

		if (delta > frameTime)
		{
			delta -= frameTime;

			Renderer::GetInstance()->TriggerRender();
		}

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
	CorsairManager* corsairManager = CorsairManager::GetInstance();

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
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
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
	
	Renderer::GetInstance()->Initialize(g_HWND);

	RAWINPUTDEVICE rid[2];
	rid[0] = { 0x01, 0x02, RIDEV_INPUTSINK, g_HWND }; // mouse
	rid[1] = { 0x01, 0x06, RIDEV_NOLEGACY | RIDEV_INPUTSINK, g_HWND }; // keyboard
	//rid[2] = { 0x01, 0x05, 0, g_HWND }; // controller
	if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0])))
	{
		DisplayMessageBox(_T("Register RawInput failed"));
		return E_FAIL;
	}
	
    ShowWindow(g_HWND, nShowCmd);
	UpdateWindow(g_HWND);

	Run();
	
	delete (corsairManager);

	return 0;
}
