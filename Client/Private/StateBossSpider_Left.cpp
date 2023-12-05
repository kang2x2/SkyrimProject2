#include "framework.h"
#include "StateBossSpider_Left.h"

#include "GameInstance.h"

#include "BossSpider.h"
#include "Player.h"

CStateBossSpider_Left::CStateBossSpider_Left()
{
}

HRESULT CStateBossSpider_Left::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateBossSpider_Left::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
	// m_fBackTime 상태 전환할 때 0으로 초기화
	if (m_fBackTime < 0.5f)
	{
		m_pMonsterTransform->Go_Backward(_fTimeDelta, m_pMonsterNavigation);
		m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

		m_fBackTime += _fTimeDelta;
	}
	else if (m_fBackTime >= 0.5f)
	{
		m_pMonsterTransform->Go_Left(_fTimeDelta, m_pMonsterNavigation);
		m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->CheckPlaySoundFile(TEXT("npc_spiderfrostbitegiant_breathe_lp.wav"), CHANNEL_MONSTER1_RUN, 1.f);

		if (pGameInstance->Collision_Enter(m_pVecCollider[CBossSpider::BOSSSPIDER_COL_ATKROUND], m_pPlayerBodyCollider))
		{
			m_pMonster->Set_State(CBossSpider::BOSSSPIDER_DOUBLECHOP);
			m_pMonster->Play_Animation(false, "attack_combochop");
		}

		Safe_Release(pGameInstance);
	}
}

void CStateBossSpider_Left::Late_Update(_float _fTimeDelta)
{
}

CStateBossSpider_Left* CStateBossSpider_Left::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateBossSpider_Left* pInstance = new CStateBossSpider_Left();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateBossSpider_Left");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateBossSpider_Left::Free()
{
}
