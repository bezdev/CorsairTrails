#include "GUI.h"

#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;

bool GUI::m_IsInitialized = false;

GUI::GUI()
{
	InitializeGDI();
}

void GUI::InitializeGDI()
{
	if (!m_IsInitialized)
	{
		// Initialize GDI+.
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		m_IsInitialized = true;
	}
}


void GUI::DrawStats(HDC hdc, BezKey* keys, int totalKeyHits, std::vector<std::pair<int, int>>& keyHistory)
{
	Graphics graphics(hdc);
	graphics.SetSmoothingMode(SmoothingMode::SmoothingModeHighSpeed);

	DrawTable(graphics, keys, totalKeyHits);
	DrawHistory(graphics, keyHistory);

	graphics.Flush(FlushIntention::FlushIntentionSync);
}

void GUI::DrawTable(Gdiplus::Graphics& graphics, BezKey* keys, int totalKeyHits)
{
	// Find all keys which were hit and sort by hit count
	std::vector<std::pair<std::wstring, BezKey>> hitKeys;
	for (int i = 0; i < InputManager::NUM_KEYS; i++)
	{
		if (keys[i].HitCount > 0)
		{
			hitKeys.push_back(make_pair(std::wstring(KeyToString(i)), keys[i]));
		}
	}

	std::sort(hitKeys.begin(), hitKeys.end(), [](std::pair<std::wstring, BezKey> k1, std::pair<std::wstring, BezKey> k2)
		{
			if (k1.second.HitCount == k2.second.HitCount)
			{
				return (k1.first < k2.first);
			}

			return (k1.second.HitCount < k2.second.HitCount);
		});

	if (hitKeys.size() == 0)
	{
		return;
	}

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

	static const int HEIGHT = 300;
	static const int WIDTH = 600;
	static const POINT POSITION = { 0, 50 };

	static const int NUM_BARS = 10;
	static const int BAR_WIDTH = 40;

	int actualNumBars = hitKeys.size() > NUM_BARS ? NUM_BARS : static_cast<int>(hitKeys.size());
	float barStep = WIDTH / (actualNumBars + 1.0f);
	POINT barStart = { POSITION.x + static_cast<LONG>(barStep), POSITION.y + static_cast<LONG>(HEIGHT) };

	// Compute max height
	int maxHeight = 0;
	int i = 0;
	int currHitCount = 0;
	int mostHit = 0;
	std::vector<std::pair<std::wstring, BezKey>>::iterator it = hitKeys.end();
	while (it != hitKeys.begin() && i < actualNumBars)
	{
		it--;

		if (i == 0)
		{
			mostHit = (*it).second.HitCount;
		}

		bool isOtherBar = (i == actualNumBars - 1) && it != hitKeys.begin();

		if (!isOtherBar)
		{
			currHitCount += (*it).second.HitCount;
		}

		i++;
	}

	maxHeight = mostHit;
	if (mostHit < totalKeyHits - currHitCount)
	{
		maxHeight = totalKeyHits - currHitCount;
	}

	i = 0;
	currHitCount = 0;
	it = hitKeys.end();
	while (it != hitKeys.begin() && i < actualNumBars)
	{
		it--;

		// If we overflow, a bar that combines all the hit counts from the other keys
		bool isOtherBar = (i == actualNumBars - 1) && it != hitKeys.begin();

		int hitCount = isOtherBar ? totalKeyHits - currHitCount : (*it).second.HitCount;
		currHitCount += hitCount;

		// Draw Bars
		int barHeight = static_cast<LONG>(hitCount / static_cast<float>(maxHeight) * HEIGHT);

		POINT barPos = { static_cast<LONG>(barStart.x - (BAR_WIDTH / 2.0) + (i * barStep)), barStart.y - barHeight };
		Rect fillRect(barPos.x, barPos.y, BAR_WIDTH, barHeight);
		SolidBrush colorBrush(Color(255, (*it).second.Color.r, (*it).second.Color.g, (*it).second.Color.b));
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
		graphics.DrawString(
			isOtherBar ? L"Other" : (*it).first.c_str(),
			static_cast<int>(isOtherBar ? 5 : (*it).first.length()),
			&font,
			PointF(static_cast<Gdiplus::REAL>(barStart.x + (i * barStep)), static_cast<Gdiplus::REAL>(barStart.y)),
			&barKeyFormat,
			&blackBrush);

		i++;
	}
	
	/*
	wchar_t buffer[50];
	swprintf(buffer, sizeof buffer / sizeof * buffer, L"barStart.y: %d\n", barStart.y);
	LOG(buffer);
	*/

	return;
}

void GUI::DrawHistory(Gdiplus::Graphics& graphics, std::vector<std::pair<int, int>>& keyHistory)
{
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
	std::vector<std::pair<int, int>>::iterator it = keyHistory.end();
	while (it != keyHistory.begin())
	{
		it--;

		std::wstring key(KeyToString((*it).first));

		graphics.DrawString(
			key.c_str(),
			static_cast<int>(key.length()),
			&font,
			PointF(20, static_cast<Gdiplus::REAL>(POSITION.y + (i * 20))),
			&historyFormat,
			&blackBrush);

		if ((*it).second > 1)
		{
			wchar_t keyHitString[10];
			_itow_s((*it).second, keyHitString, 10);
			graphics.DrawString(
				keyHitString,
				static_cast<int>(wcslen(keyHitString)),
				&tinyFont,
				PointF(20, static_cast<Gdiplus::REAL>(POSITION.y - 25 + (i * 20))),
				&historyRepeatFormat,
				&blackBrush);
		}

		i++;
	}
}
