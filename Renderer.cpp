#include "Renderer.h"

#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;

Renderer* Renderer::m_Instance;

Renderer* Renderer::GetInstance()
{
	if (m_Instance != nullptr) {
		return m_Instance;
	}

	m_Instance = new Renderer();
	return m_Instance;
}

Renderer::Renderer()
{
	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	m_HWND = nullptr;
	m_IsUpdated = false;
}

void Renderer::Initialize(HWND hwnd)
{
	m_HWND = hwnd;
}

void Renderer::TriggerUpdate()
{
	m_IsUpdated = false;
}

void Renderer::TriggerRender()
{
	if (m_IsUpdated) return;

	m_IsUpdated = true;
	InvalidateRect(m_HWND, NULL, TRUE);
}

void Renderer::Render()
{
	RECT rect;
	PAINTSTRUCT ps;
	HDC hdcBuffer;
	HDC hdc;
	HBITMAP bitmap;
	int width;
	int height;

	GetClientRect(m_HWND, &rect);
	width = rect.right - rect.left;
	height = rect.bottom + rect.left;

	hdc = BeginPaint(m_HWND, &ps);
	hdcBuffer = CreateCompatibleDC(hdc);
	bitmap = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(hdcBuffer, bitmap);

	Graphics graphics(hdcBuffer);
	graphics.SetSmoothingMode(SmoothingMode::SmoothingModeHighSpeed);

	DrawTable(graphics, width, 300, { 0, 50 });
	DrawKeys(graphics, width, height - 400, { 0, 400 });
	// DrawHistory(graphics, width, 500, { 0, 450 });

	BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);

	DeleteObject(bitmap);
	DeleteDC(hdcBuffer);
	DeleteDC(hdc);

	EndPaint(m_HWND, &ps);

	m_IsUpdated = true;
}

void Renderer::DrawTable(Graphics& graphics, int width, int height, POINT position)
{
	static SolidBrush blackBrush(Color(255, 54, 117, 229));
	static SolidBrush redBrush(Color(255, 255, 0, 0));
	static FontFamily fontFamily(L"Arial");
	static Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	static Font tinyFont(&fontFamily, 10, FontStyleRegular, UnitPixel);
	static StringFormat barValueFormat;
	barValueFormat.SetLineAlignment(StringAlignmentFar);
	barValueFormat.SetAlignment(StringAlignmentCenter);
	static StringFormat barKeyFormat;
	barKeyFormat.SetLineAlignment(StringAlignmentNear);
	barKeyFormat.SetAlignment(StringAlignmentCenter);

	static const int NUM_BARS = 10;
	static const int BAR_WIDTH = 40;

	auto hitKeys = InputManager::GetInstance()->GetSortedKeys();
	auto totalKeyHits = InputManager::GetInstance()->GetTotalKeyHits();

	int numBars = hitKeys.size() > NUM_BARS ? NUM_BARS : static_cast<int>(hitKeys.size());
	float barStep = width / (numBars + 1.0f);
	POINT barStart = { position.x + static_cast<LONG>(barStep), position.y + static_cast<LONG>(height) };

	// Compute max height
	int maxHitCount = 0;
	int currCount = 0;
	int i = 0;
	for (auto it = hitKeys.begin(); it != hitKeys.end(); it++)
	{
		// First element will have the most hits
		if (i == 0)
		{
			maxHitCount = (*it).HitCount;
		}

		// The last bar will be a combination of all remaining keys hits
		bool isOtherBar = (i == numBars - 1) && hitKeys.size() > NUM_BARS;

		if (isOtherBar)
		{
			break;
		}
		else
		{
			currCount += (*it).HitCount;
		}

		i++;
	}

	int otherCount = totalKeyHits - currCount;
	maxHitCount = maxHitCount > otherCount ? maxHitCount : otherCount;

	i = 0;
	for (auto it = hitKeys.begin(); it != hitKeys.end(); it++)
	{
		// If we overflow, a bar that combines all the hit counts from the other keys
		bool isOtherBar = (i == numBars - 1) && hitKeys.size() > NUM_BARS;

		int hitCount = isOtherBar ? otherCount : (*it).HitCount;

		// Draw Bars
		int barHeight = static_cast<LONG>(hitCount / static_cast<float>(maxHitCount)* height);

		POINT barPos = { static_cast<LONG>(barStart.x - (BAR_WIDTH / 2.0) + i * barStep), barStart.y - barHeight };
		Rect fillRect(barPos.x, barPos.y, BAR_WIDTH, barHeight);
		SolidBrush colorBrush(Color(255, (*it).Color.r, (*it).Color.g, (*it).Color.b));
		graphics.FillRectangle(&colorBrush, fillRect);

		// Draw value string
		wchar_t keyHitString[10];
		_itow_s(hitCount, keyHitString, 10);
		graphics.DrawString(
			keyHitString,
			static_cast<int>(wcslen(keyHitString)),
			&font,
			PointF(static_cast<Gdiplus::REAL>(barStart.x + (i * barStep)), static_cast<Gdiplus::REAL>(barStart.y - barHeight)),
			&barValueFormat,
			&colorBrush);

		// Draw key string
		std::wstring key(KeyToString((*it).Id));
		graphics.DrawString(
			isOtherBar ? L"Other" : key.c_str(),
			static_cast<int>(isOtherBar ? 5 : key.length()),
			& font,
			PointF(static_cast<Gdiplus::REAL>(barStart.x + (i * barStep)), static_cast<Gdiplus::REAL>(barStart.y)),
			& barKeyFormat,
			& blackBrush);

		i++;

		if (isOtherBar) break;
	}
}

struct KeyVisual
{
	BezKey Key;
	int Width;
	int Height;
};

void Renderer::DrawKeys(Graphics& graphics, int width, int height, POINT position)
{
	const int c_Padding = 10;
	const int c_KeyWidth = 40;
	const int c_KeyHeight = c_KeyWidth;

	auto addRow = [&](KeyVisual* visuals, int size, POINT startPosition)
	{
		if (size <= 0) return;

		POINT currentKeyPosition = startPosition;
		currentKeyPosition.y += visuals[0].Height / 2;
		for (int i = 0; i < size; i++)
		{
			if (i != 0)
			{
				currentKeyPosition.x += visuals[i - 1].Width / 2 + c_Padding;
			}
			currentKeyPosition.x += visuals[i].Width / 2;
			DrawKey(graphics, visuals[i].Key, visuals[i].Width, visuals[i].Height, currentKeyPosition);
		}
	};

	KeyVisual firstRowKeys[] =
	{
		{ *InputManager::GetInstance()->GetKey(VK_ESCAPE), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(VK_F1), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(VK_F2), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(VK_F3), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(VK_F4), c_KeyWidth, c_KeyHeight },
	};
	KeyVisual secondRowKeys[] =
	{
		{ *InputManager::GetInstance()->GetKey(VK_OEM_3), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x31), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x32), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x33), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x34), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x35), c_KeyWidth, c_KeyHeight },
	};
	KeyVisual thirdRowKeys[] =
	{
		{ *InputManager::GetInstance()->GetKey(VK_TAB), static_cast<int>(c_KeyWidth * 1.5f), c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x51), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x57), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x45), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x52), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x54), c_KeyWidth, c_KeyHeight },
	};
	KeyVisual fourthRowKeys[] =
	{
		{ *InputManager::GetInstance()->GetKey(VK_CAPITAL), c_KeyWidth * 2, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x41), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x53), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x44), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x46), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x47), c_KeyWidth, c_KeyHeight },
	};
	KeyVisual fifthRowKeys[] =
	{
		{ *InputManager::GetInstance()->GetKey(VK_SHIFT), static_cast<int>(c_KeyWidth * 2.5), c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x5A), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x58), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x43), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x56), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(0x42), c_KeyWidth, c_KeyHeight },
	};
	KeyVisual sixRowKeys[] =
	{
		{ *InputManager::GetInstance()->GetKey(VK_CONTROL), static_cast<int>(c_KeyWidth * 2.5), c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(VK_LWIN), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(VK_MENU), c_KeyWidth, c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(VK_SPACE), c_KeyWidth * 5, c_KeyHeight },
	};
	KeyVisual mouseKeys[] =
	{
		{ *InputManager::GetInstance()->GetKey(BezMouseButton::LEFT), c_KeyWidth, 3 * c_KeyHeight },
		{ *InputManager::GetInstance()->GetKey(BezMouseButton::RIGHT), c_KeyWidth, 3 * c_KeyHeight},
	};

	addRow(firstRowKeys, 5, { position.x, position.y });
	addRow(secondRowKeys, 6, { position.x, position.y + 1 * (c_KeyHeight + c_Padding) });
	addRow(thirdRowKeys, 6, { position.x, position.y + 2 * (c_KeyHeight + c_Padding) });
	addRow(fourthRowKeys, 6, { position.x, position.y + 3 * (c_KeyHeight + c_Padding) });
	addRow(fifthRowKeys, 6, { position.x, position.y + 4 * (c_KeyHeight + c_Padding) });
	addRow(sixRowKeys, 4, { position.x, position.y + 5 * (c_KeyHeight + c_Padding) });
	addRow(mouseKeys, 2, { width - c_KeyWidth * 4 - c_Padding, position.y });

}

void Renderer::DrawHistory(Graphics& graphics, int width, int height, POINT position)
{
	auto keyHistory = InputManager::GetInstance()->GetHistory(NUM_HISTORY);

	Pen pen(Color(255, 0, 0, 255));
	SolidBrush blackBrush(Color(255, 54, 117, 229));

	FontFamily fontFamily(L"Arial");
	Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	Font tinyFont(&fontFamily, 10, FontStyleRegular, UnitPixel);
	StringFormat barValueFormat;
	barValueFormat.SetLineAlignment(StringAlignmentFar);
	barValueFormat.SetAlignment(StringAlignmentCenter);
	StringFormat barKeyFormat;
	barKeyFormat.SetLineAlignment(StringAlignmentNear);
	barKeyFormat.SetAlignment(StringAlignmentCenter);
	StringFormat historyFormat;
	historyFormat.SetLineAlignment(StringAlignmentFar);
	historyFormat.SetAlignment(StringAlignmentNear);
	StringFormat historyRepeatFormat;
	historyRepeatFormat.SetLineAlignment(StringAlignmentNear);
	historyRepeatFormat.SetAlignment(StringAlignmentCenter);

	// Draw history
	int i = 0;
	auto it = keyHistory.end();
	while (it != keyHistory.begin())
	{
		it--;

		std::wstring key(KeyToString((*it).first.Id));

		graphics.DrawString(
			key.c_str(),
			static_cast<int>(key.length()),
			&font,
			PointF(20, static_cast<Gdiplus::REAL>(position.y + (i * 20))),
			&historyFormat,
			&blackBrush);

		if ((*it).second > 1)
		{
			std::wstring keyHitString = std::to_wstring((*it).second);
			graphics.DrawString(
				keyHitString.c_str(),
				static_cast<int>(keyHitString.length()),
				&tinyFont,
				PointF(20, static_cast<Gdiplus::REAL>(position.y - 25 + (i * 20))),
				&historyRepeatFormat,
				&blackBrush);
		}

		i++;
	}
}

void Renderer::DrawKey(Graphics& graphics, BezKey key, int width, int height, POINT position)
{
	static SolidBrush whiteBrush(Color(255, 255, 255, 255));
	static FontFamily fontFamily(L"Arial");
	static Font font(&fontFamily, 18, FontStyleRegular, UnitPixel);
	static StringFormat format;
	format.SetLineAlignment(StringAlignmentCenter);
	format.SetAlignment(StringAlignmentCenter);

	if (key.IsDown)
	{
		Rect fillRect(position.x - static_cast<int>(width / 2.0f), position.y - static_cast<int>(height / 2.0f), width, height);
		graphics.FillRectangle(&whiteBrush, fillRect);
	}

	SolidBrush brush(Color(255, key.Color.r, key.Color.g, key.Color.b));
	Pen pen(&brush);
	Rect rect(position.x - static_cast<int>(width / 2.0f), position.y - static_cast<int>(height / 2.0f), width, height);
	graphics.DrawRectangle(&pen, rect);

	// Draw key string
	std::wstring keyString(KeyToString(key.Id));
	graphics.DrawString(
		keyString.c_str(),
		static_cast<int>(keyString.length()),
		&font,
		PointF(static_cast<REAL>(position.x), static_cast<REAL>(position.y)),
		&format,
		&brush);
}
