/////////////////////////////////////////////////////////////////////
//// AppMain.cpp : Application Entry Point.
////////////////////////////////////////////////////////////////////

#pragma once

// Includes
#include "System.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	System* pSystem = new System();

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