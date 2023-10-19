#include "framework.h"
#include "Player.h"

#include "GameInstance.h"

#include "StateManager_Player.h"

#include "Player_Body.h"
#include "Weapon_IronSword.h"

CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CPlayer::Initialize_Clone(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Part()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	m_bHasMesh = true;
	m_bHasPart = true;
	m_strName = TEXT("Player");
	
	return S_OK;
}

void CPlayer::Tick(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pStateManager->Update(_fTimeDelta);

	for (auto& iter : m_vecPlayerPart)
	{
		if (iter != nullptr)
			iter->Tick(_fTimeDelta);
	}

	Safe_Release(pGameInstance);
}

void CPlayer::LateTick(_float _fTimeDelta)
{
	m_pStateManager->Late_Update();

	for (auto& iter : m_vecPlayerPart)
	{
		if (iter != nullptr)
			iter->LateTick(_fTimeDelta);
	}
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	for (auto& iter : m_vecPlayerPart)
	{
		if (iter != nullptr)
			iter->Render();
	}

	return S_OK;
}

HRESULT CPlayer::Set_State(PLAYERSTATE _eState)
{
	m_pStateManager->Set_State(_eState);

	return S_OK;
}

_bool CPlayer::Get_IsAnimationFin()
{
	return dynamic_cast<CPlayer_Body*>(m_vecPlayerPart[PART_BODY])->Get_IsAnimationFin();
}

void CPlayer::Play_Animation(_bool _bIsLoop, string _strAnimationName)
{
	dynamic_cast<CPlayer_Body*>(m_vecPlayerPart[PART_BODY])->Set_AnimationIndex(_bIsLoop, _strAnimationName);
}

HRESULT CPlayer::Ready_Component()
{
	CTransform::TRANSFORM_DESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationRadianPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Part()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pPart = nullptr;

	/* For. Player Body */
	CPlayer_Body::PART_DESC BodyPartDesc;
	BodyPartDesc.pParentTransform = m_pTransformCom;

	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Player_Body"), &BodyPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecPlayerPart.push_back(pPart);

	/* For. Weapon */
	CWeapon_IronSword::WEAPON_DESC WeaponPartDesc;
	WeaponPartDesc.pParentTransform = m_pTransformCom;
	WeaponPartDesc.pSocketBone = dynamic_cast<CPlayer_Body*>(m_vecPlayerPart[PART_BODY])->Get_SocketBonePtr("WEAPON");
	WeaponPartDesc.matSocketPivot = dynamic_cast<CPlayer_Body*>(m_vecPlayerPart[PART_BODY])->Get_SocketPivotMatrix();

	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Weapon_IronSword"), &WeaponPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecPlayerPart.push_back(pPart);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Ready_State()
{
	m_pStateManager = CStateManager_Player::Create(this, m_pTransformCom);

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResource()
{
	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayer* pInstance = new CPlayer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CPlayer ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* _pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CPlayer Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& iter : m_vecPlayerPart)
		Safe_Release(iter);

	m_vecPlayerPart.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pStateManager);
}
