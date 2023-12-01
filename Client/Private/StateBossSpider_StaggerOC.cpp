#include "framework.h"
#include "StateBossSpider_StaggerOC.h"

#include "GameInstance.h"

#include "BossSpider.h"
#include "Player.h"

CStateBossSpider_StaggerOC::CStateBossSpider_StaggerOC()
{
}

HRESULT CStateBossSpider_StaggerOC::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateBossSpider_StaggerOC::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CStateBossSpider_StaggerOC::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "recoilrchop"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CBossSpider::BOSSSPIDER_COL_ATKROUND], m_pPlayerBodyCollider))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CBossSpider::BOSSSPIDER_DOUBLECHOP);
			m_pMonster->Play_Animation(false, "attack_combochop");
		}
		else
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CBossSpider::BOSSSPIDER_CHASE);
			m_pMonster->Play_Animation(true, "forward_run");
		}
	}

	Safe_Release(pGameInstance);
}

CStateBossSpider_StaggerOC* CStateBossSpider_StaggerOC::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateBossSpider_StaggerOC* pInstance = new CStateBossSpider_StaggerOC();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateBossSpider_StaggerOC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateBossSpider_StaggerOC::Free()
{
}
