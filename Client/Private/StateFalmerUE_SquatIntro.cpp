#include "framework.h"
#include "StateFalmerUE_SquatIntro.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_SquatIntro::CStateFalmerUE_SquatIntro()
{
}

HRESULT CStateFalmerUE_SquatIntro::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_SquatIntro::Update(_float _fTimeDelta)
{
	// CGameInstance* pGameInstance = CGameInstance::GetInstance();
	// Safe_AddRef(pGameInstance);
	// 
	// CPlayer* pPlayer = dynamic_cast<CPlayer*>
	// 	(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Player")));
	// 
	// if (pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_DETECTION], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))), _fTimeDelta))
	// {
	// 	dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(true, "mtrunforward");
	// 	dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
	// }
	// 
	// Safe_Release(pGameInstance);
}

void CStateFalmerUE_SquatIntro::Late_Update()
{
	if (dynamic_cast<CMonster*>(m_pMonster)->Get_IsAnimationFin() &&
		dynamic_cast<CMonster*>(m_pMonster)->Get_CurAnimationName("idlesquatintro"))
	{
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_SQUAT);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(true, "idlesquatloop");
	}
}

CStateFalmerUE_SquatIntro* CStateFalmerUE_SquatIntro::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_SquatIntro* pInstance = new CStateFalmerUE_SquatIntro();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_SquatIntro");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_SquatIntro::Free()
{
}

