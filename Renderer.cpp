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
	DrawHistory(graphics);

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
	static StringFormat historyFormat;
	historyFormat.SetLineAlignment(StringAlignmentFar);
	historyFormat.SetAlignment(StringAlignmentNear);
	static StringFormat historyRepeatFormat;
	historyRepeatFormat.SetLineAlignment(StringAlignmentNear);
	historyRepeatFormat.SetAlignment(StringAlignmentCenter);

	static const int NUM_BARS = 10;
	static const int BAR_WIDTH = 40;

	auto hitKeys = InputManager::GetInstance()->GetSortedKeys();
	auto totalKeyHits = InputManager::GetInstance()->GetTotalKeyHits();

	int numBars = hitKeys.size() > NUM_BARS ? NUM_BARS : static_cast<int>(hitKeys.size());
	float barStep = width / (numBars + 1.0f);
	POINT barStart = { position.x + static_cast<LONG>(barStep), position.y + static_cast<LONG>(height) };
	
	/*
	Gdiplus::Pen redPen(&redBrush);
	Rect rect(position.x, position.y, position.x + width - 1, position.y + height);
	graphics.DrawRectangle(&redPen, rect);
	*/

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
	}

	return;
}

void Renderer::DrawHistory(Gdiplus::Graphics& graphics)
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

	static const POINT POSITION = { 0, 450 };

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
			PointF(20, static_cast<Gdiplus::REAL>(POSITION.y + (i * 20))),
			&historyFormat,
			&blackBrush);

		if ((*it).second > 1)
		{
			//wchar_t keyHitString[10];
			std::wstring keyHitString = std::to_wstring((*it).second);
			//_itow_s((*it).second, keyHitString, 10);
			graphics.DrawString(
				keyHitString.c_str(),
				static_cast<int>(keyHitString.length()),
				&tinyFont,
				PointF(20, static_cast<Gdiplus::REAL>(POSITION.y - 25 + (i * 20))),
				&historyRepeatFormat,
				&blackBrush);
		}

		i++;
	}
}
