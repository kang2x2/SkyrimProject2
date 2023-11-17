#include "framework.h"
#include "StateFalmerOH_StaggerL.h"

#include "GameInstance.h"

#include "Falmer_OneHand.h"
#include "Player.h"

CStateFalmerOH_StaggerL::CStateFalmerOH_StaggerL()
{
}

HRESULT CStateFalmerOH_StaggerL::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerOH_StaggerL::Update(_float _fTimeDelta)
{

}

void CStateFalmerOH_StaggerL::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hmrecoil1"))
	{
		if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_OneHand::FALMEROH_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_isReadyAtk = true;

			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}
		else
		{
			m_isReadyAtk = true;

			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_ATK2);
			m_pMonster->Play_Animation(false, "1hm_attack3");
		}
	}

	Safe_Release(pGameInstance);
}

CStateFalmerOH_StaggerL* CStateFalmerOH_StaggerL::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerOH_StaggerL* pInstance = new CStateFalmerOH_StaggerL();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerOH_StaggerL");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerOH_StaggerL::Free()
{
	__super::Free();
}
