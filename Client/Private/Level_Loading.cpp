#include "framework.h"
#include "Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"

#include "Level_Zero.h"
#include "CLevel_Tool.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVELID _eNextLevel)
{
	m_eNextLevel = _eNextLevel;

	// 다음 레벨에 대한 로딩을 수행
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevel);

	if (m_pLoader == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Tick(_float _fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Loading::LateTick(_float _fTimeDelta)
{
	wstring		strLoadingText = m_pLoader->Get_LoadingText();

	SetWindowText(g_hWnd, strLoadingText.c_str());

 	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (m_pLoader->Get_Finished() == true)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CLevel* pNewLevel = nullptr;

			switch (m_eNextLevel)
			{
			case LEVEL_ZERO:
				pNewLevel = CLevel_Zero::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_TOOL:
				pNewLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_GAMEPLAY:
				pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;
			}

			if (pNewLevel == nullptr)
				return E_FAIL;

			if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
				return E_FAIL;

			Safe_Release(pGameInstance);

		}
	}


	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_eNextLevel)))
	{
		MSG_BOX("Fail Create : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
