#include "framework.h"
#include "StateBossSpider_OneChop.h"

#include "GameInstance.h"

#include "BossSpider.h"
#include "Player.h"

CStateBossSpider_OneChop::CStateBossSpider_OneChop()
{
}

HRESULT CStateBossSpider_OneChop::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateBossSpider_OneChop::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ���� �� ������ �ݶ��̴��� �浹�Ͽ�����. (�ǰ�) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK ||
		m_pPlayer->Get_CurState() == CPlayer::ONEHAND_ANTICIPATE)
	{
		if (pGameInstance->Collision_Enter(m_pRightCollider, m_pPlayerWeaponCollider))
		{
			if (m_pPlayer->Get_IsReadyCounter())
			{
				m_pPlayer->Set_IsCounter(true);
				m_pMonster->Play_Animation(false, "recoilrchop");
				m_pMonster->Set_State(CBossSpider::BOSSSPIDER_STAGGER_ONECHOP);
			}

			m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
			m_pPlayer->Play_Animation_All(false, "1hm_blockanticipate");
		}
	}

	else
	{
		if (!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "attack_rightchop") &&
			m_pMonster->Get_CurFrameIndex() >= 16 && m_pMonster->Get_CurFrameIndex() <= 18)
		{
			if (pGameInstance->Collision_Enter(m_pRightCollider, m_pPlayerBodyCollider))
			{
				m_pPlayer->SetHp(-m_pMonster->Get_BossDesc().fChopDamage);
				m_pPlayer->Set_IsHit(true);
			}
		}
	}

	Safe_Release(pGameInstance);
}

void CStateBossSpider_OneChop::Late_Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ������ ������ �� */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "attack_rightchop"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CBossSpider::BOSSSPIDER_COL_ATKROUND], m_pPlayerBodyCollider))
		{
			m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CBossSpider::BOSSSPIDER_DOUBLECHOP);
			m_pMonster->Play_Animation(false, "attack_combochop");
		}
		else
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CBossSpider::BOSSSPIDER_CHASE);
			m_pMonster->Play_Animation(true, "forward_run");
		}
	}

	Safe_Release(pGameInstance);
}

CStateBossSpider_OneChop* CStateBossSpider_OneChop::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateBossSpider_OneChop* pInstance = new CStateBossSpider_OneChop();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateBossSpider_OneChop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateBossSpider_OneChop::Free()
{
}
