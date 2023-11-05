#pragma once

/* Ŭ���̾�Ʈ�� �����ϴ� ��� Ŭ������ ��Ŭ����ؾ��� ������ ��´�. */
#include <process.h>

namespace Client
{
	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_ZERO, 
		LEVEL_WHITERUN, LEVEL_DUNGEON,
		LEVEL_LOADING, LEVEL_TOOL, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern Client::LEVELID g_curLevel;

using namespace Client;
