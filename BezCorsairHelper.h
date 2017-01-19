#pragma once

#include "CUESDK.h"
#include <WinUser.h>

#include <algorithm>
#include <array>
#include <vector>

enum class BezEffect
{
    Random,
    HeatSpectrum
};

struct LitColor
{
    int r;
    int g;
    int b;
};

LitColor GetRandomLitColor()
{
    return { rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1 };
}

LitColor GetLitColorInSpectrum(LitColor start, LitColor end, float percent)
{
    if (percent > 100) { percent = 100; }

    int r, g, b;
    r = static_cast<int>(round(start.r + (percent / 100.f * (end.r - start.r))));
    g = static_cast<int>(round(start.g + (percent / 100.f * (end.g - start.g))));
    b = static_cast<int>(round(start.b + (percent / 100.f * (end.b - start.b))));

    return { r, g, b };
}

HRESULT GetLedsForNumber(int number, LitColor color, CorsairLedColor** ledColors, int* size)
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

CorsairLedId GetLedIdForKey(DWORD vkey, DWORD flags)
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

char* KeyToString(DWORD key)
{
    switch (key)
    {
    case 1:
        return "Escape";
    case 2:
        return "F1";
    case 3:
        return "F2";
    case 4:
        return "F3";
    case 5:
        return "F4";
    case 6:
        return "F5";
    case 7:
        return "F6";
    case 8:
        return "F7";
    case 9:
        return "F8";
    case 10:
        return "F9";
    case 11:
        return "F10";
    case 12:
        return "F11";
    case 13:
        return "GraveAccentAndTilde";
    case 14:
        return "1";
    case 15:
        return "2";
    case 16:
        return "3";
    case 17:
        return "4";
    case 18:
        return "5";
    case 19:
        return "6";
    case 20:
        return "7";
    case 21:
        return "8";
    case 22:
        return "9";
    case 23:
        return "0";
    case 24:
        return "MinusAndUnderscore";
    case 25:
        return "Tab";
    case 26:
        return "Q";
    case 27:
        return "W";
    case 28:
        return "E";
    case 29:
        return "R";
    case 30:
        return "T";
    case 31:
        return "Y";
    case 32:
        return "U";
    case 33:
        return "I";
    case 34:
        return "O";
    case 35:
        return "P";
    case 36:
        return "BracketLeft";
    case 37:
        return "CapsLock";
    case 38:
        return "A";
    case 39:
        return "S";
    case 40:
        return "D";
    case 41:
        return "F";
    case 42:
        return "G";
    case 43:
        return "H";
    case 44:
        return "J";
    case 45:
        return "K";
    case 46:
        return "L";
    case 47:
        return "SemicolonAndColon";
    case 48:
        return "ApostropheAndDoubleQuote";
    case 49:
        return "LeftShift";
    case 50:
        return "NonUsBackslash";
    case 51:
        return "Z";
    case 52:
        return "X";
    case 53:
        return "C";
    case 54:
        return "V";
    case 55:
        return "B";
    case 56:
        return "N";
    case 57:
        return "M";
    case 58:
        return "CommaAndLessThan";
    case 59:
        return "PeriodAndBiggerThan";
    case 60:
        return "SlashAndQuestionMark";
    case 61:
        return "LeftCtrl";
    case 62:
        return "LeftGui";
    case 63:
        return "LeftAlt";
    case 64:
        return "Lang2";
    case 65:
        return "Space";
    case 66:
        return "Lang1";
    case 67:
        return "International2";
    case 68:
        return "RightAlt";
    case 69:
        return "RightGui";
    case 70:
        return "Application";
    case 71:
        return "LedProgramming";
    case 72:
        return "Brightness";
    case 73:
        return "F12";
    case 74:
        return "PrintScreen";
    case 75:
        return "ScrollLock";
    case 76:
        return "PauseBreak";
    case 77:
        return "Insert";
    case 78:
        return "Home";
    case 79:
        return "PageUp";
    case 80:
        return "BracketRight";
    case 81:
        return "Backslash";
    case 82:
        return "NonUsTilde";
    case 83:
        return "Enter";
    case 84:
        return "International1";
    case 85:
        return "EqualsAndPlus";
    case 86:
        return "International3";
    case 87:
        return "Backspace";
    case 88:
        return "Delete";
    case 89:
        return "End";
    case 90:
        return "PageDown";
    case 91:
        return "RightShift";
    case 92:
        return "RightCtrl";
    case 93:
        return "UpArrow";
    case 94:
        return "LeftArrow";
    case 95:
        return "DownArrow";
    case 96:
        return "RightArrow";
    case 97:
        return "WinLock";
    case 98:
        return "Mute";
    case 99:
        return "Stop";
    case 100:
        return "ScanPreviousTrack";
    case 101:
        return "PlayPause";
    case 102:
        return "ScanNextTrack";
    case 103:
        return "NumLock";
    case 104:
        return "KeypadSlash";
    case 105:
        return "KeypadAsterisk";
    case 106:
        return "KeypadMinus";
    case 107:
        return "KeypadPlus";
    case 108:
        return "KeypadEnter";
    case 109:
        return "Keypad7";
    case 110:
        return "Keypad8";
    case 111:
        return "Keypad9";
    case 112:
        return "KeypadComma";
    case 113:
        return "Keypad4";
    case 114:
        return "Keypad5";
    case 115:
        return "Keypad6";
    case 116:
        return "Keypad1";
    case 117:
        return "Keypad2";
    case 118:
        return "Keypad3";
    case 119:
        return "Keypad0";
    case 120:
        return "KeypadPeriodAndDelete";
    case 121:
        return "G1";
    case 122:
        return "G2";
    case 123:
        return "G3";
    case 124:
        return "G4";
    case 125:
        return "G5";
    case 126:
        return "G6";
    case 127:
        return "G7";
    case 128:
        return "G8";
    case 129:
        return "G9";
    case 130:
        return "G10";
    case 131:
        return "VolumeUp";
    case 132:
        return "VolumeDown";
    case 133:
        return "MR";
    case 134:
        return "M1";
    case 135:
        return "M2";
    case 136:
        return "M3";
    case 137:
        return "G11";
    case 138:
        return "G12";
    case 139:
        return "G13";
    case 140:
        return "G14";
    case 141:
        return "G15";
    case 142:
        return "G16";
    case 143:
        return "G17";
    case 144:
        return "G18";
    case 145:
        return "International5";
    case 146:
        return "International4";
    case 147:
        return "Fn";
    default:
        return "";
    }
}