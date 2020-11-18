#include "InputManager.h"

InputManager* InputManager::Instance;

InputManager* InputManager::GetInstance()
{
	if (Instance != nullptr) {
		return Instance;
	}

	Instance = new InputManager();
	return Instance;
}

InputManager::InputManager()
{
	for (int i = 0; i < NUM_KEYS; i++)
	{
		Keys[i].Id = i;
		SortedKeys[i] = Keys[i].Id;
		SortedKeyMap[Keys[i].Id] = i;
	}
	
	for (int i = 0; i < NUM_MOUSE_BUTTONS; i++)
	{
		MouseButtons[i].Id = BezMouseButton::FIRST + i;
		SortedKeys[i + NUM_KEYS] = MouseButtons[i].Id;
		SortedKeyMap[MouseButtons[i].Id] = i + NUM_KEYS;
	}
}

void InputManager::OnKeyDown(unsigned short vkey)
{
	BezKey* key = GetKey(vkey);

	if (!key->IsDown) {
		key->HitCount++;
		key->LitTime = std::chrono::high_resolution_clock::now();

		if (ACTIVE_EFFECT == BezEffect::Random)
		{
			key->Color = GetRandomColor();
		}
		else if (ACTIVE_EFFECT == BezEffect::HeatSpectrum)
		{
			key->Color = GetColorInSpectrum(START_SPECTRUM, END_SPECTRUM, 100.f * key->HitCount / KEY_HITS_IN_SPECTRUM);
		}

#ifdef ENABLE_CUE
		if (IsKeyboardKey(vkey))
		{
			CorsairManager::GetInstance()->LightKey(vkey, key->Color);
		}
#endif

		AddKeyToHistory(vkey);
		AddKeyToSort(vkey);
		TotalKeyHits++;

		Renderer::GetInstance()->TriggerUpdate();
	}

	key->IsDown = true;
}

void InputManager::OnKeyUp(unsigned short vkey)
{
	GetKey(vkey)->IsDown = false;

	Renderer::GetInstance()->TriggerUpdate();
}

void InputManager::ProcessRawInput(WPARAM wParam, LPARAM lParam)
{
	UINT dwSize = 0;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if (lpb == NULL)
	{
		return;
	}

	auto s = scope_exit([lpb]() {
		delete[] lpb;
	});

	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
	{
		return;
	}

	RAWINPUT* raw = (RAWINPUT*)lpb;
	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0)
		{
			InputManager::OnKeyDown(BezMouseButton::LEFT);
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) != 0)
		{
			InputManager::OnKeyUp(BezMouseButton::LEFT);
		}

		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0)
		{
			InputManager::OnKeyDown(BezMouseButton::MIDDLE);
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0)
		{
			InputManager::OnKeyUp(BezMouseButton::MIDDLE);
		}

		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0)
		{
			InputManager::OnKeyDown(BezMouseButton::RIGHT);
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) != 0)
		{
			InputManager::OnKeyUp(BezMouseButton::RIGHT);
		}

		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) != 0)
		{
			InputManager::OnKeyDown(BezMouseButton::M4);
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) != 0)
		{
			InputManager::OnKeyUp(BezMouseButton::M4);
		}

		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) != 0)
		{
			InputManager::OnKeyDown(BezMouseButton::M5);
		}
		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) != 0)
		{
			InputManager::OnKeyUp(BezMouseButton::M5);
		}
	}
	else if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		if (raw->data.keyboard.Flags == 0 || raw->data.keyboard.Flags == 2)
		{
			OnKeyDown(raw->data.keyboard.VKey);
		}
		else if (raw->data.keyboard.Flags == 1 || raw->data.keyboard.Flags == 3)
		{
			OnKeyUp(raw->data.keyboard.VKey);
		}
	}
	else if (raw->header.dwType == RIM_TYPEHID)
	{
		LOG(_T("controller"));
	}
}

std::vector<std::pair<BezKey, int>> InputManager::GetHistory(int count)
{
	std::vector<std::pair<BezKey, int>> result;

	KeyHistoryMutex.lock();

	if (KeyHistory.size() > 0)
	{
		int numKeys = 0;
		for (auto key = KeyHistory.rbegin(); key != KeyHistory.rend(); key++)
		{
			if (result.size() > 0 && result.back().first.Id == *key) {
				result.back().second++;
				continue;
			}

			std::pair<BezKey, int> p{ *GetKey(*key), 1 };
			result.push_back(p);

			numKeys++;
			if (numKeys == count) break;
		}
	}

	KeyHistoryMutex.unlock();

	std::reverse(result.begin(), result.end());
	return result;
}

std::vector<BezKey> InputManager::GetSortedKeys()
{
	// Find all keys which were hit and sort by hit count
	std::vector<BezKey> hitKeys;

	for (int i = 0; i < NUM_KEYS + NUM_MOUSE_BUTTONS; i++)
	{
		if (GetKey(SortedKeys[i])->HitCount == 0) break;

		hitKeys.push_back(*GetKey(SortedKeys[i]));
	}

	return hitKeys;
}
 
void InputManager::AddKeyToHistory(unsigned short vkey)
{
	KeyHistoryMutex.lock();

	KeyHistory.push_back(vkey);
	
	if (KeyHistory.size() > InputManager::NUM_HISTORY)
	{
		KeyHistory.erase(KeyHistory.begin());
	}

	KeyHistoryMutex.unlock();
}

void InputManager::AddKeyToSort(unsigned short vkey)
{
	int indexInSorted = SortedKeyMap[vkey];
	while (indexInSorted > 0) {
		BezKey* ahead = GetKey(SortedKeys[indexInSorted - 1]);
		if (ahead->HitCount <= GetKey(SortedKeys[indexInSorted])->HitCount)
		{
			// swap
			SortedKeys[indexInSorted - 1] = vkey;
			SortedKeys[indexInSorted] = ahead->Id;

			SortedKeyMap[ahead->Id] = indexInSorted;
			SortedKeyMap[vkey] = indexInSorted - 1;

			indexInSorted--;
		}
		else
		{
			break;
		}
	}
}

BezKey* const InputManager::GetKey(unsigned short vkey)
{
	if (BezMouseButton::FIRST <= vkey && vkey <= BezMouseButton::LAST)
	{
		return &MouseButtons[vkey - BezMouseButton::FIRST];
	}

	return &Keys[vkey];
}

int InputManager::GetTotalKeyHits()
{
	return TotalKeyHits;
}

bool InputManager::IsKeyboardKey(unsigned short vkey)
{
	return 0 <= vkey && vkey <= NUM_KEYS;
}
