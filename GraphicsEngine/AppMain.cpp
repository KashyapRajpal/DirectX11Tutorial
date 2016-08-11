/////////////////////////////////////////////////////////////////////
//// AppMain.cpp : Application Entry Point.
////////////////////////////////////////////////////////////////////

#pragma once

// Includes
#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* pSystem = new SystemClass();

	if (pSystem == nullptr)
	{
		return 0;
	}

	bool bInit = pSystem->Init();

	if (bInit)
	{
		pSystem->Run();
	}

	pSystem->Release();

	delete pSystem;

	pSystem = nullptr;

	return 0;
}