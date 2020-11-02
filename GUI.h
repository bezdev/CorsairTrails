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

class GUI
{
public:
	GUI();
	static void GUI::InitializeGDI();
	void GUI::DrawStats(HDC hdc, BezKey* keys, int totalKeyHits, std::vector<std::pair<int, int>>& keyHistory);
private:
	static bool m_IsInitialized;
	void GUI::DrawTable(Gdiplus::Graphics& graphics, BezKey* keys, int totalKeyHits);
	void GUI::DrawHistory(Gdiplus::Graphics& graphics, std::vector<std::pair<int, int>>& keyHistory);
};