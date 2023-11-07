#include "framework.h"
#include "StateFalmerUE_Warning.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_Warning::CStateFalmerUE_Warning()
{
}

HRESULT CStateFalmerUE_Warning::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Warning::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>
		(pGameInstance->Find_CloneObject(LEVEL_WHITERUN, TEXT("Layer_Player"), TEXT("Player")));

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

	m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);
}

void CStateFalmerUE_Warning::Late_Update()
{
	if (dynamic_cast<CMonster*>(m_pMonster)->Get_IsAnimationFin() &&
		dynamic_cast<CMonster*>(m_pMonster)->Get_CurAnimationName("1hm_aggrowarning1"))
	{
		m_pMonsterTransform->Set_Speed(dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->GetRunSpeed());

		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(true, "mtrunforward");
	}
}

CStateFalmerUE_Warning* CStateFalmerUE_Warning::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Warning* pInstance = new CStateFalmerUE_Warning();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Warning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Warning::Free()
{
}

