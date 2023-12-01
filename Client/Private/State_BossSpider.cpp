#include "framework.h"
#include "State_BossSpider.h"

#include "GameInstance.h"
#include "BossSpider.h"

CState_BossSpider::CState_BossSpider()
{
}

HRESULT CState_BossSpider::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	m_pMonster = dynamic_cast<CBossSpider*>(_pMonster);
	m_pMouthCollider = dynamic_cast<CCollider*>(m_pMonster->Get_Part(CBossSpider::PART_MOUTH)->Get_Component(TEXT("Com_Collider_OBB")));
	m_pLeftCollider = dynamic_cast<CCollider*>(m_pMonster->Get_Part(CBossSpider::PART_LEFT)->Get_Component(TEXT("Com_Collider_OBB")));
	m_pRightCollider = dynamic_cast<CCollider*>(m_pMonster->Get_Part(CBossSpider::PART_RIGHT)->Get_Component(TEXT("Com_Collider_OBB")));

	if (FAILED(__super::Initialize(_pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
		return E_FAIL;

	return S_OK;
}

void CState_BossSpider::Update(_float _fTimeDelta)
{
}

void CState_BossSpider::Late_Update()
{
}

void CState_BossSpider::Free()
{
}
