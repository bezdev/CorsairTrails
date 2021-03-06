#include "CorsairManager.h"

CorsairManager* CorsairManager::Instance;

CorsairManager* CorsairManager::GetInstance()
{
	if (Instance != nullptr) {
		return Instance;
	}

	Instance = new CorsairManager();
	if (FAILED(Instance->Initialize())) {
		return nullptr;
	}

	return Instance;
}

CorsairManager::CorsairManager()
{

}

HRESULT CorsairManager::Initialize()
{
	HRESULT hr = S_OK;

	CorsairPerformProtocolHandshake();

	CorsairError error = CorsairGetLastError();
	if (error != CE_Success)
	{
		std::cerr << "Handshake failed: " << error << std::endl;
		return E_FAIL;
	}

	if (!CorsairGetDeviceCount())
	{
		std::cerr << "No devices found" << std::endl;
		return E_FAIL;
	}

	CorsairRequestControl(CAM_ExclusiveLightingControl);
	
	srand(static_cast<unsigned int>(time(NULL)));

	auto winLockColor = CorsairLedColor{ CLK_WinLock, 255, 0, 0 };
	auto brightnessColor = CorsairLedColor{ CLK_Brightness, 255, 255, 255 };
	auto logoColor = CorsairLedColor{ CLK_Logo, 255, 0, 0 };

	CorsairSetLedsColorsAsync(1, &winLockColor, nullptr, nullptr);
	CorsairSetLedsColorsAsync(1, &brightnessColor, nullptr, nullptr);
	CorsairSetLedsColorsAsync(1, &logoColor, nullptr, nullptr);

	return S_OK;
}

void CorsairManager::LightKey(unsigned short vkey, BezColor color)
{
	CorsairLedId ledId = GetLedIdForKey(vkey);
	CorsairLedColor ledColor = CorsairLedColor{ ledId, color.r, color.g, color.b };
	CorsairSetLedsColorsAsync(1, &ledColor, nullptr, nullptr);
}
