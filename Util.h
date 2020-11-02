#pragma once

#include <Windows.h>
#include <WinUser.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <chrono>
#include <string>
#include <thread>
#include <sstream>

#include "CUESDK.h"

#define DEBUG

#ifdef DEBUG
#define LOG(msg, ...) do { wchar_t buffer[100]; swprintf(buffer, 100, msg, __VA_ARGS__); auto id = std::this_thread::get_id(); std::wstringstream ss; ss << id << ":" << buffer << std::endl; auto str = ss.str(); const wchar_t* cstr = str.c_str(); OutputDebugStringW(cstr); } while(0)
#else
#define LOG
#endif

enum BezMouseButton {
	LEFT,
	MIDDLE,
	RIGHT,
	M4,
	M5,
	BMB_LAST = M5
};

enum class BezEffect
{
    Random,
    HeatSpectrum
};

struct BezColor
{
	int r;
	int g;
	int b;
};

struct BezKey
{
	bool IsLit;
	bool IsDown;
	unsigned int HitCount;
	// TODO: change
	std::chrono::high_resolution_clock::time_point LitTime;
	struct BezColor Color;

	void Reset()
	{
		IsLit = false;
		IsDown = false;
		HitCount = 0;
		Color = { 0 };
	}
};

static BezColor GetRandomColor()
{
    return { rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1 };
}

static BezColor GetColorInSpectrum(BezColor start, BezColor end, float percent)
{
    if (percent > 100) { percent = 100; }

    int r, g, b;
    r = static_cast<int>(round(start.r + (percent / 100.f * (end.r - start.r))));
    g = static_cast<int>(round(start.g + (percent / 100.f * (end.g - start.g))));
    b = static_cast<int>(round(start.b + (percent / 100.f * (end.b - start.b))));

    return { r, g, b };
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

static CorsairLedId GetLedIdForKey(unsigned short vkey, DWORD flags)
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
        return (flags == 0x1) ? CLK_KeypadEnter : CLK_Enter;
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
        return (flags == 0x1) ? CLK_Insert : CLK_Keypad0;
    case VK_HOME:
        return (flags == 0x1) ? CLK_Home : CLK_Keypad7;
    case VK_PRIOR:
        return (flags == 0x1) ? CLK_PageUp : CLK_Keypad9;
    case VK_DELETE:
        return (flags == 0x1) ? CLK_Delete : CLK_KeypadPeriodAndDelete;
    case VK_END:
        return (flags == 0x1) ? CLK_End : CLK_Keypad1;
    case VK_NEXT:
        return (flags == 0x1) ? CLK_PageDown : CLK_Keypad3;
    case VK_LEFT:
        return (flags == 0x1) ? CLK_LeftArrow : CLK_Keypad4;
    case VK_UP:
        return (flags == 0x1) ? CLK_UpArrow : CLK_Keypad8;
    case VK_RIGHT:
        return (flags == 0x1) ? CLK_RightArrow : CLK_Keypad6;
    case VK_DOWN:
        return (flags == 0x1) ? CLK_DownArrow : CLK_Keypad2;
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

static wchar_t* KeyToString(DWORD key)
{
    switch (key)
    {
    case 1:
        return L"Esc";
    case 2:
        return L"F1";
    case 3:
        return L"F2";
    case 4:
        return L"F3";
    case 5:
        return L"F4";
    case 6:
        return L"F5";
    case 7:
        return L"F6";
    case 8:
        return L"F7";
    case 9:
        return L"F8";
    case 10:
        return L"F9";
    case 11:
        return L"F10";
    case 12:
        return L"F11";
    case 13:
        return L"`~";
    case 14:
        return L"1";
    case 15:
        return L"2";
    case 16:
        return L"3";
    case 17:
        return L"4";
    case 18:
        return L"5";
    case 19:
        return L"6";
    case 20:
        return L"7";
    case 21:
        return L"8";
    case 22:
        return L"9";
    case 23:
        return L"0";
    case 24:
        return L"-_";
    case 25:
        return L"Tab";
    case 26:
        return L"Q";
    case 27:
        return L"W";
    case 28:
        return L"E";
    case 29:
        return L"R";
    case 30:
        return L"T";
    case 31:
        return L"Y";
    case 32:
        return L"U";
    case 33:
        return L"I";
    case 34:
        return L"O";
    case 35:
        return L"P";
    case 36:
        return L"[{";
    case 37:
        return L"C Lock";
    case 38:
        return L"A";
    case 39:
        return L"S";
    case 40:
        return L"D";
    case 41:
        return L"F";
    case 42:
        return L"G";
    case 43:
        return L"H";
    case 44:
        return L"J";
    case 45:
        return L"K";
    case 46:
        return L"L";
    case 47:
        return L";:";
    case 48:
        return L"'\"";
    case 49:
        return L"L Shift";
    case 50:
        return L"|";
    case 51:
        return L"Z";
    case 52:
        return L"X";
    case 53:
        return L"C";
    case 54:
        return L"V";
    case 55:
        return L"B";
    case 56:
        return L"N";
    case 57:
        return L"M";
    case 58:
        return L",<";
    case 59:
        return L".>";
    case 60:
        return L"/?";
    case 61:
        return L"L Ctrl";
    case 62:
        return L"WIN";
    case 63:
        return L"L Alt";
    case 64:
        return L"Lang2";
    case 65:
        return L"Space";
    case 66:
        return L"Lang1";
    case 67:
        return L"International2";
    case 68:
        return L"R Alt";
    case 69:
        return L"R WIN";
    case 70:
        return L"App";
    case 71:
        return L"LedProgramming";
    case 72:
        return L"Brightness";
    case 73:
        return L"F12";
    case 74:
        return L"P Screen";
    case 75:
        return L"S Lock";
    case 76:
        return L"Break";
    case 77:
        return L"Ins";
    case 78:
        return L"Home";
    case 79:
        return L"PageUp";
    case 80:
        return L"]}";
    case 81:
        return L"Backslash";
    case 82:
        return L"NonUsTilde";
    case 83:
        return L"Enter";
    case 84:
        return L"International1";
    case 85:
        return L"EqualsAndPlus";
    case 86:
        return L"International3";
    case 87:
        return L"Backspace";
    case 88:
        return L"Delete";
    case 89:
        return L"End";
    case 90:
        return L"PageDown";
    case 91:
        return L"RightShift";
    case 92:
        return L"RightCtrl";
    case 93:
        return L"UpArrow";
    case 94:
        return L"LeftArrow";
    case 95:
        return L"DownArrow";
    case 96:
        return L"RightArrow";
    case 97:
        return L"WinLock";
    case 98:
        return L"Mute";
    case 99:
        return L"Stop";
    case 100:
        return L"ScanPreviousTrack";
    case 101:
        return L"PlayPause";
    case 102:
        return L"ScanNextTrack";
    case 103:
        return L"NumLock";
    case 104:
        return L"KeypadSlash";
    case 105:
        return L"KeypadAsterisk";
    case 106:
        return L"KeypadMinus";
    case 107:
        return L"KeypadPlus";
    case 108:
        return L"KeypadEnter";
    case 109:
        return L"Keypad7";
    case 110:
        return L"Keypad8";
    case 111:
        return L"Keypad9";
    case 112:
        return L"KeypadComma";
    case 113:
        return L"Keypad4";
    case 114:
        return L"Keypad5";
    case 115:
        return L"Keypad6";
    case 116:
        return L"Keypad1";
    case 117:
        return L"Keypad2";
    case 118:
        return L"Keypad3";
    case 119:
        return L"Keypad0";
    case 120:
        return L"KeypadPeriodAndDelete";
    case 121:
        return L"G1";
    case 122:
        return L"G2";
    case 123:
        return L"G3";
    case 124:
        return L"G4";
    case 125:
        return L"G5";
    case 126:
        return L"G6";
    case 127:
        return L"G7";
    case 128:
        return L"G8";
    case 129:
        return L"G9";
    case 130:
        return L"G10";
    case 131:
        return L"VolumeUp";
    case 132:
        return L"VolumeDown";
    case 133:
        return L"MR";
    case 134:
        return L"M1";
    case 135:
        return L"M2";
    case 136:
        return L"M3";
    case 137:
        return L"G11";
    case 138:
        return L"G12";
    case 139:
        return L"G13";
    case 140:
        return L"G14";
    case 141:
        return L"G15";
    case 142:
        return L"G16";
    case 143:
        return L"G17";
    case 144:
        return L"G18";
    case 145:
        return L"International5";
    case 146:
        return L"International4";
    case 147:
        return L"Fn";
	case 171:
		return L"LMB";
	case 170:
		return L"RMB";
    default:
        return L"";
    }
}

template <typename T>
class lambda_call
{
public:
	explicit lambda_call(T&& lambda) : m_Lambda(std::move(lambda))
	{
	}

	~lambda_call()
	{
		m_Lambda();
	}
protected:
	T m_Lambda;
};

template <typename T>
inline auto scope_exit(T&& lambda)
{
	return lambda_call<T>(std::forward<T>(lambda));
}
