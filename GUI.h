#pragma once

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

class GUI
{
public:
	GUI();
	void GUI::DrawStats(HDC hdc, BezKey* keys, int totalKeyHits, std::vector<std::pair<int, int>>& keyHistory);
private:
	
};