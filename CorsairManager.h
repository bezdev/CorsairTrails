#pragma once

#include "CUESDK.h"

#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include <stdlib.h> 
#include <tchar.h>  
#include <gdiplus.h>

#include "Util.h"

class CorsairManager
{
public:
	CorsairManager();
	HRESULT Initialize();
};