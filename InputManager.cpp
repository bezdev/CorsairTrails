#include "InputManager.h"

InputManager::InputManager()
{
}

void InputManager::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT kbd = *((KBDLLHOOKSTRUCT*)lParam);
	auto ledId = GetLedIdForKey(kbd.vkCode, kbd.flags);

	if (!Keys[ledId].isDown)
	{
		// Light up corsair key
		if (ledId != CLI_Invalid)
		{
			if (ACTIVE_EFFECT == BezEffect::Random)
			{
				Keys[ledId].litColor = GetRandomColor();
			}
			else if (ACTIVE_EFFECT == BezEffect::HeatSpectrum)
			{
				Keys[ledId].litColor = GetColorInSpectrum(START_SPECTRUM, END_SPECTRUM, 100.f * Keys[ledId].hitCount / KEY_HITS_IN_SPECTRUM);
			}

#ifdef DEBUG
			std::cout << _corsairKeys[ledId].litColor.r << "," << _corsairKeys[ledId].litColor.g << "," << _corsairKeys[ledId].litColor.b << std::endl;
#endif

			auto ledColor = CorsairLedColor{ ledId, Keys[ledId].litColor.r, Keys[ledId].litColor.g, Keys[ledId].litColor.b };
			CorsairSetLedsColorsAsync(1, &ledColor, nullptr, nullptr);

			Keys[ledId].litTime = std::chrono::high_resolution_clock::now();
			Keys[ledId].isLit = true;
		}
#ifdef DEBUG
		else
		{
			std::cout << "CLI_Invalid" << std::endl;
		}
#endif

		// Update history
		if (KeyHistory.size() > 0 &&
			KeyHistory.back().first == ledId)
		{
			KeyHistory.back().second++;
		}
		else
		{
			KeyHistory.push_back(std::make_pair<int, int>(static_cast<int>(ledId), 1));
		}

		if (KeyHistory.size() > HISTORY_SIZE)
		{
			KeyHistory.erase(KeyHistory.begin());
		}

		Keys[ledId].hitCount++;
		TotalKeyHits++;
	}

	Keys[ledId].isDown = true;
}

void InputManager::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT kbd = *((KBDLLHOOKSTRUCT*)lParam);
	auto ledId = GetLedIdForKey(kbd.vkCode, kbd.flags);

	Keys[ledId].isDown = false;
}
