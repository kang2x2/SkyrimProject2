#include "framework.h"
#include "StateFalmerUE_Squat.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_Squat::CStateFalmerUE_Squat()
{
}

HRESULT CStateFalmerUE_Squat::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Squat::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>
		(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Player")));

	if (pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_DETECTION], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(false, "idlesquatoutro");
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_SQUAT_OUTRO);
	}

	Safe_Release(pGameInstance);
}

void CStateFalmerUE_Squat::Late_Update()
{

}

CStateFalmerUE_Squat* CStateFalmerUE_Squat::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Squat* pInstance = new CStateFalmerUE_Squat();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Squat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Squat::Free()
{
}

