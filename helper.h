#pragma once

#include "CUESDK.h"
#include <WinUser.h>

CorsairLedId GetLedIdForKey(DWORD vkey, DWORD flags)
{
	switch (vkey)
	{
	case VK_ESCAPE:
		return CLK_Escape;
		break;
	case VK_F1:
		return CLK_F1;
		break;
	case VK_F2:
		return CLK_F2;
		break;
	case VK_F3:
		return CLK_F3;
		break;
	case VK_F4:
		return CLK_F4;
		break;
	case VK_F5:
		return CLK_F5;
		break;
	case VK_F6:
		return CLK_F6;
		break;
	case VK_F7:
		return CLK_F7;
		break;
	case VK_F8:
		return CLK_F8;
		break;
	case VK_F9:
		return CLK_F9;
		break;
	case VK_F10:
		return CLK_F10;
		break;
	case VK_F11:
		return CLK_F11;
		break;
	case VK_F12:
		return CLK_F12;
		break;
	case VK_OEM_3:
		return CLK_GraveAccentAndTilde;
		break;
	case 0x31:
		return CLK_1;
		break;
	case 0x32:
		return CLK_2;
		break;
	case 0x33:
		return CLK_3;
		break;
	case 0x34:
		return CLK_4;
		break;
	case 0x35:
		return CLK_5;
		break;
	case 0x36:
		return CLK_6;
		break;
	case 0x37:
		return CLK_7;
		break;
	case 0x38:
		return CLK_8;
		break;
	case 0x39:
		return CLK_9;
		break;
	case 0x30:
		return CLK_0;
		break;
	case VK_TAB:
		return CLK_Tab;
		break;
	case VK_CAPITAL:
		return CLK_CapsLock;
		break;
	case VK_LSHIFT:
		return CLK_LeftShift;
		break;
	case VK_RSHIFT:
		return CLK_RightShift;
		break;
	case VK_LCONTROL:
		return CLK_LeftCtrl;
		break;
	case VK_RCONTROL:
		return CLK_RightCtrl;
		break;
	case VK_LWIN:
		return CLK_LeftGui;
		break;
	case VK_LMENU:
		return CLK_LeftAlt;
		break;
	case VK_RMENU:
		return CLK_RightAlt;
		break;
	case VK_SPACE:
		return CLK_Space;
		break;
	case VK_APPS:
		return CLK_Application;
		break;
	case VK_OEM_MINUS:
		return CLK_MinusAndUnderscore;
		break;
	case VK_OEM_PLUS:
		return CLK_EqualsAndPlus;
		break;
	case VK_BACK:
		return CLK_Backspace;
		break;
	case VK_OEM_4:
		return CLK_BracketLeft;
		break;
	case VK_OEM_6:
		return CLK_BracketRight;
		break;
	case VK_OEM_5:
		return CLK_Backslash;
		break;
	case VK_OEM_1:
		return CLK_SemicolonAndColon;
		break;
	case VK_OEM_7:
		return CLK_ApostropheAndDoubleQuote;
		break;
	case VK_RETURN:
		return (flags == 0x1) ? CLK_KeypadEnter : CLK_Enter;
		break;
	case VK_OEM_COMMA:
		return CLK_CommaAndLessThan;
		break;
	case VK_OEM_PERIOD:
		return CLK_PeriodAndBiggerThan;
		break;
	case VK_OEM_2:
		return CLK_SlashAndQuestionMark;
		break;
	case VK_SNAPSHOT:
		return CLK_PrintScreen;
		break;
	case VK_SCROLL:
		return CLK_ScrollLock;
		break;
	case VK_PAUSE:
		return CLK_PauseBreak;
		break;
	case VK_INSERT:
		return CLK_Insert;
		break;
	case VK_HOME:
		return CLK_Home;
		break;
	case VK_PRIOR:
		return CLK_PageUp;
		break;
	case VK_DELETE:
		return (flags == 0x1) ? CLK_Delete : CLK_KeypadPeriodAndDelete;
		break;
	case VK_END:
		return CLK_End;
		break;
	case VK_NEXT:
		return CLK_PageDown;
		break;
	case VK_LEFT:
		return CLK_LeftArrow;
		break;
	case VK_UP:
		return CLK_UpArrow;
		break;
	case VK_RIGHT:
		return CLK_RightArrow;
		break;
	case VK_DOWN:
		return CLK_DownArrow;
		break;
	case VK_NUMLOCK:
		return CLK_NumLock;
		break;
	case VK_DIVIDE:
		return CLK_KeypadSlash;
		break;
	case VK_MULTIPLY:
		return CLK_KeypadAsterisk;
		break;
	case VK_SUBTRACT:
		return CLK_KeypadMinus;
		break;
	case VK_ADD:
		return CLK_KeypadPlus;
		break;
	case VK_DECIMAL:
		return CLK_KeypadPeriodAndDelete;
		break;
	case VK_NUMPAD0:
		return CLK_Keypad0;
		break;
	case VK_NUMPAD1:
		return CLK_Keypad1;
		break;
	case VK_NUMPAD2:
		return CLK_Keypad2;
		break;
	case VK_NUMPAD3:
		return CLK_Keypad3;
		break;
	case VK_NUMPAD4:
		return CLK_Keypad4;
		break;
	case VK_NUMPAD5:
		return CLK_Keypad5;
		break;
	case VK_NUMPAD6:
		return CLK_Keypad6;
		break;
	case VK_NUMPAD7:
		return CLK_Keypad7;
		break;
	case VK_NUMPAD8:
		return CLK_Keypad8;
		break;
	case VK_NUMPAD9:
		return CLK_Keypad9;
		break;
	default:
		return CorsairGetLedIdForKeyName(static_cast<char>(vkey));
		break;
	}
}