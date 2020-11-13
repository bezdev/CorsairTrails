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
#define ENABLE_CUE

#define RETURN_IF_FAILED(hr) do { if (FAILED(hr)) { return -1; } } while(0)

#ifdef DEBUG
#define LOG(msg, ...) do { wchar_t buffer[100]; swprintf(buffer, 100, msg, __VA_ARGS__); auto id = std::this_thread::get_id(); std::wstringstream ss; ss << id << ":" << buffer << std::endl; auto str = ss.str(); const wchar_t* cstr = str.c_str(); OutputDebugStringW(cstr); } while(0)
#else
#define LOG
#endif

enum BezMouseButton {
	LEFT = 420,
	MIDDLE,
	RIGHT,
	M4,
	M5,
	FIRST = LEFT,
	LAST = M5
};

struct BezColor
{
	int r;
	int g;
	int b;
};

struct BezKey
{
	unsigned short Id;
	bool IsDown;
	unsigned int HitCount;
	// TODO: change
	std::chrono::high_resolution_clock::time_point LitTime;
	struct BezColor Color;

	void Reset()
	{
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

static wchar_t* KeyToString(unsigned short vkey)
{
	switch (vkey)
	{
		case VK_LBUTTON: return L"LMB";
		case VK_RBUTTON: return L"RMB";
		case VK_CANCEL: return L"CANCEL";
		case VK_MBUTTON: return L"MMB";
		case VK_XBUTTON1: return L"X1MB";
		case VK_XBUTTON2: return L"X2MB";
		case VK_BACK: return L"BACKSPACE";
		case VK_TAB: return L"TAB";
		case VK_CLEAR: return L"CLEAR";
		case VK_RETURN: return L"ENTER";
		case VK_SHIFT: return L"SHIFT";
		case VK_CONTROL: return L"CTRL";
		case VK_MENU: return L"ALT";
		case VK_PAUSE: return L"PAUSE";
		case VK_CAPITAL: return L"CAPSLOCK";
		case VK_KANA: return L"IME Kana mode";
		case VK_JUNJA: return L"IME Junja mode";
		case VK_FINAL: return L"IME final mode";
		case VK_HANJA: return L"IME Hanja mode";
		case VK_ESCAPE: return L"ESC";
		case VK_CONVERT: return L"IME convert";
		case VK_NONCONVERT: return L"IME nonconvert";
		case VK_ACCEPT: return L"IME accept";
		case VK_MODECHANGE: return L"IME mode change request";
		case VK_SPACE: return L"SPACEBAR";
		case VK_PRIOR: return L"PAGE UP";
		case VK_NEXT: return L"PAGE DOWN";
		case VK_END: return L"END";
		case VK_HOME: return L"HOME";
		case VK_LEFT: return L"LEFT ARROW";
		case VK_UP: return L"UP ARROW";
		case VK_RIGHT: return L"RIGHT ARROW";
		case VK_DOWN: return L"DOWN ARROW";
		case VK_SELECT: return L"SELECT";
		case VK_PRINT: return L"PRINT";
		case VK_EXECUTE: return L"EXECUTE";
		case VK_SNAPSHOT: return L"PRINT SCREEN";
		case VK_INSERT: return L"INS";
		case VK_DELETE: return L"DEL";
		case VK_HELP: return L"HELP";
		case 0x30: return L"0";
		case 0x31: return L"1";
		case 0x32: return L"2";
		case 0x33: return L"3";
		case 0x34: return L"4";
		case 0x35: return L"5";
		case 0x36: return L"6";
		case 0x37: return L"7";
		case 0x38: return L"8";
		case 0x39: return L"9";
		case 0x41: return L"A";
		case 0x42: return L"B";
		case 0x43: return L"C";
		case 0x44: return L"D";
		case 0x45: return L"E";
		case 0x46: return L"F";
		case 0x47: return L"G";
		case 0x48: return L"H";
		case 0x49: return L"I";
		case 0x4A: return L"J";
		case 0x4B: return L"K";
		case 0x4C: return L"L";
		case 0x4D: return L"M";
		case 0x4E: return L"N";
		case 0x4F: return L"O";
		case 0x50: return L"P";
		case 0x51: return L"Q";
		case 0x52: return L"R";
		case 0x53: return L"S";
		case 0x54: return L"T";
		case 0x55: return L"U";
		case 0x56: return L"V";
		case 0x57: return L"W";
		case 0x58: return L"X";
		case 0x59: return L"Y";
		case 0x5A: return L"Z";
		case VK_LWIN: return L"Left Windows key (Natural keyboard)";
		case VK_RWIN: return L"Right Windows key (Natural keyboard)";
		case VK_APPS: return L"Applications key (Natural keyboard)";
		case VK_SLEEP: return L"Computer Sleep";
		case VK_NUMPAD0: return L"NUM 0";
		case VK_NUMPAD1: return L"NUM 1";
		case VK_NUMPAD2: return L"NUM 2";
		case VK_NUMPAD3: return L"NUM 3";
		case VK_NUMPAD4: return L"NUM 4";
		case VK_NUMPAD5: return L"NUM 5";
		case VK_NUMPAD6: return L"NUM 6";
		case VK_NUMPAD7: return L"NUM 7";
		case VK_NUMPAD8: return L"NUM 8";
		case VK_NUMPAD9: return L"NUM 9";
		case VK_MULTIPLY: return L"*";
		case VK_ADD: return L"+=";
		case VK_SEPARATOR: return L"/";
		case VK_SUBTRACT: return L"-";
		case VK_DECIMAL: return L".";
		case VK_DIVIDE: return L"/";
		case VK_F1: return L"F1";
		case VK_F2: return L"F2";
		case VK_F3: return L"F3";
		case VK_F4: return L"F4";
		case VK_F5: return L"F5";
		case VK_F6: return L"F6";
		case VK_F7: return L"F7";
		case VK_F8: return L"F8";
		case VK_F9: return L"F9";
		case VK_F10: return L"F10";
		case VK_F11: return L"F11";
		case VK_F12: return L"F12";
		case VK_F13: return L"F13";
		case VK_F14: return L"F14";
		case VK_F15: return L"F15";
		case VK_F16: return L"F16";
		case VK_F17: return L"F17";
		case VK_F18: return L"F18";
		case VK_F19: return L"F19";
		case VK_F20: return L"F20";
		case VK_F21: return L"F21";
		case VK_F22: return L"F22";
		case VK_F23: return L"F23";
		case VK_F24: return L"F24";
		case VK_NUMLOCK: return L"NUM LOCK";
		case VK_SCROLL: return L"SCROLL LOCK";
		case VK_LSHIFT: return L"Left SHIFT";
		case VK_RSHIFT: return L"Right SHIFT";
		case VK_LCONTROL: return L"Left CONTROL";
		case VK_RCONTROL: return L"Right CONTROL";
		case VK_LMENU: return L"Left MENU";
		case VK_RMENU: return L"Right MENU";
		case VK_BROWSER_BACK: return L"Browser Back";
		case VK_BROWSER_FORWARD: return L"Browser Forward";
		case VK_BROWSER_REFRESH: return L"Browser Refresh";
		case VK_BROWSER_STOP: return L"Browser Stop";
		case VK_BROWSER_SEARCH: return L"Browser Search";
		case VK_BROWSER_FAVORITES: return L"Browser Favorites";
		case VK_BROWSER_HOME: return L"Browser Start and Home";
		case VK_VOLUME_MUTE: return L"Volume Mute";
		case VK_VOLUME_DOWN: return L"Volume Down";
		case VK_VOLUME_UP: return L"Volume Up";
		case VK_MEDIA_NEXT_TRACK: return L"Next Track";
		case VK_MEDIA_PREV_TRACK: return L"Previous Track";
		case VK_MEDIA_STOP: return L"Stop Media";
		case VK_MEDIA_PLAY_PAUSE: return L"Play/Pause Media";
		case VK_LAUNCH_MAIL: return L"Start Mail";
		case VK_LAUNCH_MEDIA_SELECT: return L"Select Media";
		case VK_LAUNCH_APP1: return L"Start Application 1";
		case VK_LAUNCH_APP2: return L"Start Application 2";
		case VK_OEM_1: return L";:";
		case VK_OEM_PLUS: return L"+=";
		case VK_OEM_COMMA: return L",<";
		case VK_OEM_MINUS: return L"-=";
		case VK_OEM_PERIOD: return L".>";
		case VK_OEM_2: return L"/?";
		case VK_OEM_3: return L"`~";
		case VK_OEM_4: return L"[{";
		case VK_OEM_5: return L"\\|";
		case VK_OEM_6: return L"]}";
		case VK_OEM_7: return L"'\"";
		case VK_OEM_8: return L"OEM";
		case 0xE1: return L"OEM specific";
		case VK_OEM_102: return L"\\|";
		case VK_PROCESSKEY: return L"IME PROCESS";
		case VK_PACKET: return L"Packet";
		case VK_ATTN: return L"Attn";
		case VK_CRSEL: return L"CrSel";
		case VK_EXSEL: return L"ExSel";
		case VK_EREOF: return L"Erase EOF";
		case VK_PLAY: return L"Play";
		case VK_ZOOM: return L"Zoom";
		case VK_NONAME: return L"Reserved";
		case VK_PA1: return L"PA1";
		case VK_OEM_CLEAR: return L"Clear";
		case BezMouseButton::LEFT: return L"LMB";
		case BezMouseButton::RIGHT: return L"RMB";
		case BezMouseButton::MIDDLE: return L"MMB";
		case BezMouseButton::M4: return L"M4";
		case BezMouseButton::M5: return L"M5";
		default: 
			LOG(L"UNKNOWN VKEY: %d\n", vkey);
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
