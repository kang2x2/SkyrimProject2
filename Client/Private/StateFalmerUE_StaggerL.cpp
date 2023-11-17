#include "framework.h"
#include "StateFalmerUE_StaggerL.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"
#include "Player.h"

CStateFalmerUE_StaggerL::CStateFalmerUE_StaggerL()
{
}

HRESULT CStateFalmerUE_StaggerL::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_StaggerL::Update(_float _fTimeDelta)
{

}

void CStateFalmerUE_StaggerL::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hmrecoil1"))
	{
		m_isReadyAtk = true;

		if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}
		else
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_ATK2);
			m_pMonster->Play_Animation(false, "1hm_attack3");
		}
	}

	Safe_Release(pGameInstance);
}

CStateFalmerUE_StaggerL* CStateFalmerUE_StaggerL::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_StaggerL* pInstance = new CStateFalmerUE_StaggerL();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_StaggerL");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_StaggerL::Free()
{
	__super::Free();
}
