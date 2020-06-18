#include "GUI.h"

#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;

GUI::GUI()
{

}

void GUI::DrawStats(HDC hdc, BezKey* keys, int totalKeyHits, std::vector<std::pair<int, int>>& keyHistory)
{
	std::vector<std::pair<std::wstring, BezKey>> hitKeys;

	// Sort key counts
	for (int i = 0; i < InputManager::NUM_KEYS; i++)
	{
		if (keys[i].hitCount > 0)
		{
			hitKeys.push_back(make_pair(std::wstring(KeyToString(i)), keys[i]));
		}
	}

	std::sort(hitKeys.begin(), hitKeys.end(), [](std::pair<std::wstring, BezKey> k1, std::pair<std::wstring, BezKey> k2)
		{
			if (k1.second.hitCount == k2.second.hitCount)
			{
				return (k1.first < k2.first);
			}

			return (k1.second.hitCount < k2.second.hitCount);
		});

	if (hitKeys.size() == 0)
	{
		return;
	}

	Graphics graphics(hdc);
	graphics.SetSmoothingMode(SmoothingMode::SmoothingModeHighSpeed);
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

	static const int HEIGHT = 300;
	static const int WIDTH = 600;
	static const POINT POSITION = { 0, 50 };

	static const int NUM_BARS = 10;
	static const int BAR_WIDTH = 40;

	int actualNumBars = hitKeys.size() > NUM_BARS ? NUM_BARS : static_cast<int>(hitKeys.size());
	float barStep = WIDTH / (actualNumBars + 1.0f);
	POINT barStart = { POSITION.x + barStep, POSITION.y + HEIGHT };

	{
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
				mostHit = (*it).second.hitCount;
			}

			bool isOtherBar = (i == actualNumBars - 1) && it != hitKeys.begin();

			if (!isOtherBar)
			{
				currHitCount += (*it).second.hitCount;
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

			SolidBrush colorBrush(Color(255, (*it).second.litColor.r, (*it).second.litColor.g, (*it).second.litColor.b));

			// If we overflow, a bar that combines all the hit counts from the other keys
			bool isOtherBar = (i == actualNumBars - 1) && it != hitKeys.begin();

			int hitCount = isOtherBar ? totalKeyHits - currHitCount : (*it).second.hitCount;
			currHitCount += hitCount;

			// Draw Bars
			int barHeight = hitCount / static_cast<float>(maxHeight)* HEIGHT;

			POINT barPos = { barStart.x - (BAR_WIDTH / 2.0) + (i * barStep), barStart.y - barHeight };
			Rect fillRect(barPos.x, barPos.y, BAR_WIDTH, barHeight);
			graphics.FillRectangle(&colorBrush, fillRect);

			// Draw value string
			wchar_t keyHitString[10];
			_itow(hitCount, keyHitString, 10);
			graphics.DrawString(
				keyHitString,
				wcslen(keyHitString),
				&font,
				PointF(barStart.x + (i * barStep), barStart.y - barHeight),
				&barValueFormat,
				&colorBrush);

			// Draw key string
			graphics.DrawString(
				isOtherBar ? L"Other" : (*it).first.c_str(),
				isOtherBar ? 5 : (*it).first.length(),
				&font,
				PointF(barStart.x + (i * barStep), barStart.y),
				&barKeyFormat,
				&blackBrush);

			i++;
		}
	}

	// Draw history
	{
		int i = 0;
		std::vector<std::pair<int, int>>::iterator it = keyHistory.end();
		while (it != keyHistory.begin())
		{
			it--;

			std::wstring key(KeyToString((*it).first));

			graphics.DrawString(
				key.c_str(),
				key.length(),
				&font,
				PointF(20, barStart.y + 100 + (i * 20)),
				&historyFormat,
				&blackBrush);

			if ((*it).second > 1)
			{
				wchar_t keyHitString[10];
				_itow((*it).second, keyHitString, 10);
				graphics.DrawString(
					keyHitString,
					wcslen(keyHitString),
					&tinyFont,
					PointF(20, barStart.y + 75 + (i * 20)),
					&historyRepeatFormat,
					&blackBrush);
			}

			i++;
		}
	}

	graphics.Flush(FlushIntention::FlushIntentionSync);
}
