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
	static CorsairManager* GetInstance();

	CorsairManager();
	HRESULT Initialize();
	void LightKey(unsigned short vkey, BezColor color);
private:
	static CorsairManager* Instance;
};

static CorsairLedId GetLedIdForKey(unsigned short vkey)
{
	switch (vkey)
	{
	case VK_ESCAPE:
		return CLK_Escape;
	case VK_F1:
		return CLK_F1;
	case VK_F2:
		return CLK_F2;
	case VK_F3:
		return CLK_F3;
	case VK_F4:
		return CLK_F4;
	case VK_F5:
		return CLK_F5;
	case VK_F6:
		return CLK_F6;
	case VK_F7:
		return CLK_F7;
	case VK_F8:
		return CLK_F8;
	case VK_F9:
		return CLK_F9;
	case VK_F10:
		return CLK_F10;
	case VK_F11:
		return CLK_F11;
	case VK_F12:
		return CLK_F12;
	case VK_OEM_3:
		return CLK_GraveAccentAndTilde;
	case 0x31:
		return CLK_1;
	case 0x32:
		return CLK_2;
	case 0x33:
		return CLK_3;
	case 0x34:
		return CLK_4;
	case 0x35:
		return CLK_5;
	case 0x36:
		return CLK_6;
	case 0x37:
		return CLK_7;
	case 0x38:
		return CLK_8;
	case 0x39:
		return CLK_9;
	case 0x30:
		return CLK_0;
	case VK_TAB:
		return CLK_Tab;
	case VK_CAPITAL:
		return CLK_CapsLock;
	case VK_LSHIFT:
		return CLK_LeftShift;
	case VK_RSHIFT:
		return CLK_RightShift;
	case VK_LCONTROL:
		return CLK_LeftCtrl;
	case VK_RCONTROL:
		return CLK_RightCtrl;
	case VK_LWIN:
		return CLK_LeftGui;
	case VK_LMENU:
		return CLK_LeftAlt;
	case VK_RMENU:
		return CLK_RightAlt;
	case VK_SPACE:
		return CLK_Space;
	case VK_APPS:
		return CLK_Application;
	case VK_OEM_MINUS:
		return CLK_MinusAndUnderscore;
	case VK_OEM_PLUS:
		return CLK_EqualsAndPlus;
	case VK_BACK:
		return CLK_Backspace;
	case VK_OEM_4:
		return CLK_BracketLeft;
	case VK_OEM_6:
		return CLK_BracketRight;
	case VK_OEM_5:
		return CLK_Backslash;
	case VK_OEM_1:
		return CLK_SemicolonAndColon;
	case VK_OEM_7:
		return CLK_ApostropheAndDoubleQuote;
	case VK_RETURN:
		return CLK_Enter;
	case VK_OEM_COMMA:
		return CLK_CommaAndLessThan;
	case VK_OEM_PERIOD:
		return CLK_PeriodAndBiggerThan;
	case VK_OEM_2:
		return CLK_SlashAndQuestionMark;
	case VK_SNAPSHOT:
		return CLK_PrintScreen;
	case VK_SCROLL:
		return CLK_ScrollLock;
	case VK_PAUSE:
		return CLK_PauseBreak;
	case VK_INSERT:
		return CLK_Insert;
	case VK_HOME:
		return CLK_Home;
	case VK_PRIOR:
		return CLK_PageUp;
	case VK_DELETE:
		return CLK_Delete;
	case VK_END:
		return CLK_End;
	case VK_NEXT:
		return CLK_PageDown;
	case VK_LEFT:
		return CLK_LeftArrow;
	case VK_UP:
		return CLK_UpArrow;
	case VK_RIGHT:
		return CLK_RightArrow;
	case VK_DOWN:
		return CLK_DownArrow;
	case VK_NUMLOCK:
		return CLK_NumLock;
	case VK_DIVIDE:
		return CLK_KeypadSlash;
	case VK_MULTIPLY:
		return CLK_KeypadAsterisk;
	case VK_SUBTRACT:
		return CLK_KeypadMinus;
	case VK_ADD:
		return CLK_KeypadPlus;
	case VK_DECIMAL:
		return CLK_KeypadPeriodAndDelete;
	case VK_NUMPAD0:
		return CLK_Keypad0;
	case VK_NUMPAD1:
		return CLK_Keypad1;
	case VK_NUMPAD2:
		return CLK_Keypad2;
	case VK_NUMPAD3:
		return CLK_Keypad3;
	case VK_NUMPAD4:
		return CLK_Keypad4;
	case VK_NUMPAD5:
	case VK_CLEAR:
		return CLK_Keypad5;
	case VK_NUMPAD6:
		return CLK_Keypad6;
	case VK_NUMPAD7:
		return CLK_Keypad7;
	case VK_NUMPAD8:
		return CLK_Keypad8;
	case VK_NUMPAD9:
		return CLK_Keypad9;
	default:
		return CorsairGetLedIdForKeyName(static_cast<char>(vkey));
	}
}

static HRESULT GetLedsForNumber(int number, BezColor color, CorsairLedColor** ledColors, int* size)
{
	std::array<CorsairLedId, 14> numberLeds{
		CLK_NumLock,   CLK_KeypadSlash,  CLK_KeypadAsterisk,
		CLK_Keypad7,   CLK_Keypad8,      CLK_Keypad9,
		CLK_Keypad4,   CLK_Keypad5,      CLK_Keypad6,
		CLK_Keypad1,   CLK_Keypad2,      CLK_Keypad3,
		CLK_Keypad0, /*CLK_Keypad0*/     CLK_KeypadPeriodAndDelete
	};

	std::vector<CorsairLedId> onLeds;

	switch (number)
	{
	case 1:
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_Keypad9);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		break;
	case 2:
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_KeypadSlash);
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_Keypad9);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad5);
		onLeds.push_back(CLK_Keypad4);
		onLeds.push_back(CLK_Keypad1);
		onLeds.push_back(CLK_Keypad0);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		break;
	case 3:
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_KeypadSlash);
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_Keypad9);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad5);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		onLeds.push_back(CLK_Keypad0);
		break;
	case 4:
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_Keypad7);
		onLeds.push_back(CLK_Keypad4);
		onLeds.push_back(CLK_Keypad5);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad9);
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		break;
	case 5:
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_KeypadSlash);
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_Keypad7);
		onLeds.push_back(CLK_Keypad4);
		onLeds.push_back(CLK_Keypad5);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		onLeds.push_back(CLK_Keypad0);
		break;
	case 6:
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_KeypadSlash);
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_Keypad7);
		onLeds.push_back(CLK_Keypad4);
		onLeds.push_back(CLK_Keypad5);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		onLeds.push_back(CLK_Keypad0);
		onLeds.push_back(CLK_Keypad1);
		break;
	case 7:
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_KeypadSlash);
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_Keypad9);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		break;
	case 8:
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_KeypadSlash);
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_Keypad9);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		onLeds.push_back(CLK_Keypad0);
		onLeds.push_back(CLK_Keypad1);
		onLeds.push_back(CLK_Keypad4);
		onLeds.push_back(CLK_Keypad7);
		onLeds.push_back(CLK_Keypad5);
		break;
	case 9:
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_KeypadSlash);
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_Keypad9);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		onLeds.push_back(CLK_Keypad4);
		onLeds.push_back(CLK_Keypad7);
		onLeds.push_back(CLK_Keypad5);
		onLeds.push_back(CLK_Keypad0);
		break;
	case 0:
		onLeds.push_back(CLK_NumLock);
		onLeds.push_back(CLK_KeypadSlash);
		onLeds.push_back(CLK_KeypadAsterisk);
		onLeds.push_back(CLK_Keypad9);
		onLeds.push_back(CLK_Keypad6);
		onLeds.push_back(CLK_Keypad3);
		onLeds.push_back(CLK_KeypadPeriodAndDelete);
		onLeds.push_back(CLK_Keypad0);
		onLeds.push_back(CLK_Keypad1);
		onLeds.push_back(CLK_Keypad4);
		onLeds.push_back(CLK_Keypad7);
		break;
	}

	*size = static_cast<int>(numberLeds.size());
	*ledColors = static_cast<CorsairLedColor*>(malloc(*size * sizeof(CorsairLedColor)));
	if (*ledColors == nullptr)
	{
		return E_OUTOFMEMORY;
	}

	for (std::array<CorsairLedId, 14>::size_type i = 0; i < numberLeds.size(); i++)
	{
		(*ledColors + i)->ledId = numberLeds[i];
	}

	for (int i = 0; i < *size; i++)
	{
		if (std::find(std::begin(onLeds), std::end(onLeds), (*ledColors + i)->ledId) != std::end(onLeds))
		{
			(*ledColors + i)->r = color.r;
			(*ledColors + i)->g = color.g;
			(*ledColors + i)->b = color.b;
		}
		else
		{
			(*ledColors + i)->r = 0;
			(*ledColors + i)->g = 0;
			(*ledColors + i)->b = 255;
		}
	}

	return S_OK;
}