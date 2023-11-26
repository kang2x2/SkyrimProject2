#include "framework.h"
#include "StateSpider_Spit.h"

#include "GameInstance.h"

#include "Spider.h"
#include "Player.h"

CStateSpider_Spit::CStateSpider_Spit()
{
}

HRESULT CStateSpider_Spit::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateSpider_Spit::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);
}

void CStateSpider_Spit::Late_Update()
{
	if (m_pMonster->Get_CurFrameIndex() == 8)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		_vector vSpitPos = m_pMonsterTransform->Get_State(CTransform::STATE_POSITION);

		// 거미줄 발사
		pGameInstance->Add_CloneObject(g_curLevel,
			TEXT("Projectile"), TEXT("ProtoType_GameObject_CProjectile_Web"), &vSpitPos);
	
		Safe_Release(pGameInstance);
	}
}

CStateSpider_Spit* CStateSpider_Spit::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateSpider_Spit* pInstance = new CStateSpider_Spit();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateSpider_Spit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateSpider_Spit::Free()
{
}

