#include "Level_Manager.h"
#include "Level.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

void CLevel_Manager::Tick(_float _fTimeDelta)
{
	if (m_pCurLevel == nullptr)
		return;

	m_pCurLevel->Tick(_fTimeDelta);
}

void CLevel_Manager::LateTick(_float _fTimeDelta)
{
	if (m_pCurLevel == nullptr)
		return;

	m_pCurLevel->LateTick(_fTimeDelta);
}

void CLevel_Manager::AfterRender()
{
	if (m_pCurLevel == nullptr)
		return;

	m_pCurLevel->AfterRender();
}

HRESULT CLevel_Manager::Open_Level(_uint _iLevelIndex, CLevel* _pLevel)
{
	// �̹� ������ �����ϸ� �ı�
	if (m_pCurLevel != nullptr)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Clear(m_iCurLevelIdx);

		Safe_Release(pGameInstance);

		int i = 0;
	}

	Safe_Release(m_pCurLevel);
	m_pCurLevel = _pLevel;
	m_iCurLevelIdx = _iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pCurLevel);
}
