#include "framework.h"
#include "StateFalmerUE_Return.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_Return::CStateFalmerUE_Return()
{
}

HRESULT CStateFalmerUE_Return::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Return::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// z ∞Ì¡§.


	CPlayer* pPlayer = dynamic_cast<CPlayer*>
		(pGameInstance->Find_CloneObject(LEVEL_WHITERUN, TEXT("Layer_Player"), TEXT("Player")));

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

	// m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);
	
	m_pMonsterTransform->Chase(dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos(), _fTimeDelta, 1.5f);
	m_pMonsterTransform->LookAt(dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos());

	if (pGameInstance->Collision_DetectionPlayer(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_DETECTION], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))), _fTimeDelta))
	{
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_WARNING);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(false, "1hm_aggrowarning1");
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerUE_Return::Late_Update()
{
	_float4 vPos = {};
	_float4 vOriginPos = {};

	XMStoreFloat4(&vPos, m_pMonsterTransform->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&vOriginPos, dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos());

	if (vPos.x >= vOriginPos.x - 1.f && vPos.x <= vOriginPos.x + 1.f &&
		vPos.z >= vOriginPos.z - 2.f && vPos.z <= vOriginPos.z + 2.f)
	{
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_SQUAT_INTRO);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(false, "idlesquatintro");
	}
}

CStateFalmerUE_Return* CStateFalmerUE_Return::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Return* pInstance = new CStateFalmerUE_Return();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Return");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Return::Free()
{
}

