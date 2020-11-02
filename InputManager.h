#pragma once

#include "Util.h"

class InputManager
{
public:
	const BezEffect ACTIVE_EFFECT = BezEffect::HeatSpectrum;
	const BezColor START_SPECTRUM = { 0, 191, 255 };
	const BezColor END_SPECTRUM = { 255, 0, 255 };
	static const unsigned int KEY_HITS_IN_SPECTRUM = 150;
	static const unsigned int HISTORY_SIZE = 10;
	// TODO: refactors keys to be indexed by vkey
	static const unsigned int NUM_KEYS = CLI_Last + 1 + /*hack:*/ 2;
	static const unsigned int NUM_MOUSE_BUTTONS = BMB_LAST + 1;

	// hack
	static const unsigned int RMB_ID = CLI_Last + 1;
	static const unsigned int LMB_ID = CLI_Last + 2;

	InputManager();

	void OnKeyDown(unsigned short vkey);
	void OnKeyUp(unsigned short vkey);
	void OnMouseDown(WPARAM wParam, LPARAM lParam);
	void OnMouseUp(WPARAM wParam, LPARAM lParam);

	void ProcessRawInput(WPARAM wParam, LPARAM lParam);

	void UpdateHistory(int id);

	BezKey* GetKeys();
	std::vector<std::pair<int, int>>& GetKeyHistory() { return KeyHistory; };
	int GetTotalKeyHits();
		
private:
	BezKey Keys[NUM_KEYS] = { 0 };
	BezKey MouseButtons[NUM_MOUSE_BUTTONS] = { 0 };
	std::vector<std::pair<int, int>> KeyHistory;
	int TotalKeyHits = 0;
};