#include "framework.h"
#include "StateFalmerUE_StaggerH.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"
#include "Player.h"

CStateFalmerUE_StaggerH::CStateFalmerUE_StaggerH()
{
}

HRESULT CStateFalmerUE_StaggerH::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_StaggerH::Update(_float _fTimeDelta)
{
	if (m_pMonster->Get_CurFrameIndex() < 40)
		m_pMonsterTransform->Go_Backward(_fTimeDelta, m_pMonsterNavigation);
}

void CStateFalmerUE_StaggerH::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hmstaggerbackheavy"))
	{
		if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_isReadyAtk = true;

			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}
		else
		{
			m_isReadyAtk = true;

			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_ATK);
			m_pMonster->Play_Animation(false, "1hm_attack1");
		}
	}

	Safe_Release(pGameInstance);
}

CStateFalmerUE_StaggerH* CStateFalmerUE_StaggerH::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_StaggerH* pInstance = new CStateFalmerUE_StaggerH();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_StaggerH");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_StaggerH::Free()
{
	__super::Free();
}
