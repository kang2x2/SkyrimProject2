#include "framework.h"
#include "StateFalmerUE_SquatOutro.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_SquatOutro::CStateFalmerUE_SquatOutro()
{
}

HRESULT CStateFalmerUE_SquatOutro::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_SquatOutro::Update(_float _fTimeDelta)
{
	// CGameInstance* pGameInstance = CGameInstance::GetInstance();
	// Safe_AddRef(pGameInstance);
	// 
	// CPlayer* pPlayer = dynamic_cast<CPlayer*>
	// 	(pGameInstance->Find_CloneObject(LEVEL_WHITERUN, TEXT("Layer_Player"), TEXT("Player")));
	// 
	// if (pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_DETECTION], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))), _fTimeDelta))
	// {
	// 	dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(true, "mtrunforward");
	// 	dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
	// }
	// 
	// Safe_Release(pGameInstance);
}

void CStateFalmerUE_SquatOutro::Late_Update()
{
	if (dynamic_cast<CMonster*>(m_pMonster)->Get_IsAnimationFin())
	{
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_WARNING);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(false, "1hm_aggrowarning1");
	}
}

CStateFalmerUE_SquatOutro* CStateFalmerUE_SquatOutro::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_SquatOutro* pInstance = new CStateFalmerUE_SquatOutro();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_SquatOutro");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_SquatOutro::Free()
{
}

