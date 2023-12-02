#include "framework.h"
#include "StateBossSpider_StaggerDC.h"

#include "GameInstance.h"

#include "BossSpider.h"
#include "Player.h"

CStateBossSpider_StaggerDC::CStateBossSpider_StaggerDC()
{
}

HRESULT CStateBossSpider_StaggerDC::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateBossSpider_StaggerDC::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CStateBossSpider_StaggerDC::Late_Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "recoillchop"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CBossSpider::BOSSSPIDER_COL_ATKROUND], m_pPlayerBodyCollider))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CBossSpider::BOSSSPIDER_BITE);
			m_pMonster->Play_Animation(false, "attack_bitelow");
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

CStateBossSpider_StaggerDC* CStateBossSpider_StaggerDC::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateBossSpider_StaggerDC* pInstance = new CStateBossSpider_StaggerDC();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateBossSpider_StaggerDC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateBossSpider_StaggerDC::Free()
{
}
