#pragma once

#include <chrono>

#include "BezCorsairHelper.h"

class InputManager
{
public:
	const BezEffect ACTIVE_EFFECT = BezEffect::HeatSpectrum;
	const BezColor START_SPECTRUM = { 0, 191, 255 };
	const BezColor END_SPECTRUM = { 255, 0, 255 };
	static const unsigned int KEY_HITS_IN_SPECTRUM = 150;
	static const unsigned int HISTORY_SIZE = 10;
	static const unsigned int NUM_KEYS = CLI_Last + 1;

	InputManager();

	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);

	BezKey* GetKeys() { return Keys; }
	std::vector<std::pair<int, int>>& GetKeyHistory() { return KeyHistory; };
	int GetTotalKeyHits() { return TotalKeyHits; };

private:
	BezKey Keys[NUM_KEYS] = { 0 };
	std::vector<std::pair<int, int>> KeyHistory;
	int TotalKeyHits = 0;
};