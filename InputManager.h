#pragma once

#include "Util.h"
#include "Renderer.h"
#include "CorsairManager.h"
#include <mutex>
#include <map>


enum class BezEffect
{
	Random,
	HeatSpectrum
};

class InputManager
{
public:
	static InputManager* GetInstance();

	const BezEffect ACTIVE_EFFECT = BezEffect::HeatSpectrum;
	const BezColor START_SPECTRUM = { 0, 191, 255 };
	const BezColor END_SPECTRUM = { 255, 0, 255 };
	static const unsigned int KEY_HITS_IN_SPECTRUM = 150;

	static const unsigned int NUM_KEYS = 256;
	static const unsigned int NUM_MOUSE_BUTTONS = BezMouseButton::LAST - BezMouseButton::FIRST + 1;
	static const unsigned int NUM_HISTORY = 100;

	InputManager();

	void OnKeyDown(unsigned short vkey);
	void OnKeyUp(unsigned short vkey);

	void ProcessRawInput(WPARAM wParam, LPARAM lParam);

	std::vector<std::pair<BezKey, int>> GetHistory(int count);
	std::vector<BezKey> GetSortedKeys();

	BezKey* const GetKey(unsigned short vkey);
	bool IsKeyboardKey(unsigned short vkey);

	int GetTotalKeyHits();
		
private:
	static InputManager* Instance;

	void AddKeyToHistory(unsigned short vkey);
	void AddKeyToSort(unsigned short vkey);

	BezKey Keys[NUM_KEYS] = { 0 };
	BezKey MouseButtons[NUM_MOUSE_BUTTONS] = { 0 };

	unsigned short SortedKeys[NUM_KEYS + NUM_MOUSE_BUTTONS] = { 0 };
	std::map<unsigned short, unsigned short> SortedKeyMap;

	std::mutex KeyHistoryMutex;
	std::vector<unsigned short> KeyHistory;

	int TotalKeyHits = 0;
};
