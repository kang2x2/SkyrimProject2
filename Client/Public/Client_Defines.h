#pragma once

/* Ŭ���̾�Ʈ�� �����ϴ� ��� Ŭ������ ��Ŭ����ؾ��� ������ ��´�. */
#include <process.h>

namespace Client
{
	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_ZERO, 
		LEVEL_WHITERUN, LEVEL_DUNGEON,
		LEVEL_LOADING, LEVEL_TOOL, LEVEL_END };

	enum PLAYERSTATE {
		ONEHAND_IDLE,
		ONEHAND_RUN_F, ONEHAND_RUN_B, ONEHAND_RUN_L, ONEHAND_RUN_R,
		ONEHAND_LATTACK, ONEHAND_RATTACK, ONEHAND_PATTACK, ONEHAND_RUNPOWERATTACK,
		ONEHAND_END
	};


	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;
