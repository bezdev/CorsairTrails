#pragma once

#include <windows.h>
#include <iostream>
#include <thread>
#include <string.h>
#include <stdlib.h> 
#include <tchar.h>  
#include <gdiplus.h>
#include <string>

#include "InputManager.h"
#include "CorsairManager.h"
#include "Util.h"

class Renderer
{
public:
	static const int NUM_HISTORY = 10;

	static Renderer* GetInstance();

	Renderer();

	void Renderer::Initialize(HWND hwnd);
	void Renderer::TriggerRender();
	void Renderer::TriggerUpdate();
	void Renderer::Render();
private:
	static Renderer* m_Instance;

	HWND m_HWND;
	bool m_IsUpdated;

	void Renderer::DrawTable(Gdiplus::Graphics& graphics, int width, int height, POINT position);
	void Renderer::DrawKeys(Gdiplus::Graphics& graphics, int width, int height, POINT position);
	void Renderer::DrawHistory(Gdiplus::Graphics& graphics, int width, int height, POINT position);
	void Renderer::DrawKey(Gdiplus::Graphics& graphics, BezKey key, int width, int height, POINT position);
};
