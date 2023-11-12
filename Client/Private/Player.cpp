#include "framework.h"
#include "Player.h"

#include "GameInstance.h"

#include "StateManager_Player.h"

#include "Player_Weapon.h"

#include "Player_CameraPart.h"
#include "Player_Body.h"
#include "Player_Weapon.h"
#include "Player_Armor.h"
#include "Player_Helmet.h"

CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCreatureObject(_pDevice, _pContext)
{
	for (_int i = 0; i < STAGE_END; ++i)
	{
		m_pNavigationCom[i] = nullptr;
	}
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCreatureObject(rhs)
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

	m_bIsMaintain = true;
	m_bHasMesh = true;
	m_bHasPart = true;
	m_strName = TEXT("Player");
	m_eEquipState = EQUIP_UNEQUIP;

	m_pTransformCom->Set_Speed(m_fRunSpeed);

	Play_Animation(true, "mt_idle");

	m_eCurCamMode = CAM_1ST;
	//m_eCurCamMode = CAM_3ST;

	return S_OK;
}

void CPlayer::PriorityTick(_float _fTimeDelta)
{
	for (auto& iter : m_vecPlayerPart)
	{
		if (iter != nullptr)
			iter->PriorityTick(_fTimeDelta);
	}
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

	_vector	vPosition = m_pNavigationCom[g_curStage]->Set_OnCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

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

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
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

#ifdef _DEBUG
	m_pNavigationCom[g_curStage]->Render();
#endif

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
_bool CPlayer::Get_CurAnimationName(string _strAnimationName)
{
	return dynamic_cast<CPlayer_Body*>(m_vecPlayerPart[PART_BODY])->Get_CurAnimationName(_strAnimationName);
}
_uint CPlayer::Get_CurFrameIndex()
{
	return dynamic_cast<CPlayer_Body*>(m_vecPlayerPart[PART_BODY])->Get_CurFrameIndex();
}
const char* CPlayer::Get_CurSocketBonName()
{
	return dynamic_cast<CPlayer_Weapon*>(m_vecPlayerPart[PART_WEAPON])->Get_SoketBoneName();
}
void CPlayer::Play_Animation(_bool _bIsLoop, string _strAnimationName)
{
	dynamic_cast<CPlayer_Body*>(m_vecPlayerPart[PART_BODY])->Set_AnimationIndex(_bIsLoop, _strAnimationName);
}

void CPlayer::Set_SoketBone(const char* _pBoneName)
{
	dynamic_cast<CPlayer_Weapon*>(m_vecPlayerPart[PART_WEAPON])
		->Set_SoketBone(dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])
			->Get_SocketBonePtr(_pBoneName));
}
_vector CPlayer::Get_PlayerCamLook()
{
	return dynamic_cast<CPlayer_CameraPart*>(m_vecPlayerPart[PART_CAMERA])->Get_PlayerCamLook();
}
void CPlayer::Set_PlayerCam()
{
	if (m_eCurCamMode == CAM_1ST)
		m_eCurCamMode = CAM_3ST;
	else if (m_eCurCamMode == CAM_3ST)
		m_eCurCamMode = CAM_1ST;

	CPlayer_Body* pBody = dynamic_cast<CPlayer_Body*>(m_vecPlayerPart[PART_BODY]);
	pBody->Set_BodyType(m_eCurCamMode);

	if(m_eCurCamMode == CAM_1ST)
		dynamic_cast<CPlayer_CameraPart*>(m_vecPlayerPart[PART_CAMERA])->Set_SoketBone(
			pBody->Get_SocketBonePtr("Camera1st [Cam1]"));
	else if(m_eCurCamMode == CAM_3ST)
		dynamic_cast<CPlayer_CameraPart*>(m_vecPlayerPart[PART_CAMERA])->Set_SoketBone(
			pBody->Get_SocketBonePtr("Camera3rd [Cam3]"));

	dynamic_cast<CPlayer_CameraPart*>(m_vecPlayerPart[PART_CAMERA])->Set_PivotMatrix(
		pBody->Get_SocketPivotMatrix());

}

void CPlayer::Set_CurCell()
{
	m_pNavigationCom[g_curStage]->Set_CurCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CPlayer::CheckHit_Onehand(_uint _iSourFrame, _uint _iDestFrame)
{
	dynamic_cast<CPlayer_Weapon*>(m_vecPlayerPart[PART_WEAPON])->CheckHit_Onehand(_iSourFrame, _iDestFrame);
}

HRESULT CPlayer::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), &TransformDesc))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::DESC_NAVIGATION		NavigationDesc;
	NavigationDesc.iCurIndex = 0;
	
	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Navigation_WhiteRun"),
		TEXT("Com_Navigation_WhiteRun"), (CComponent**)&m_pNavigationCom[STAGE_WHITERUN], &NavigationDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Navigation_Dungeon"),
		TEXT("Com_Navigation_Dungeon"), (CComponent**)&m_pNavigationCom[STAGE_DUNGEON], &NavigationDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Part()
{	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, -1.3f, 12.f, 1.f));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pPart = nullptr;

	/* For. Player Body */
	CPlayer_Body::PART_DESC BodyPartDesc;
	BodyPartDesc.pParent = this;
	BodyPartDesc.pParentTransform = m_pTransformCom;

	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Player_Body"), &BodyPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecPlayerPart.push_back(pPart);

	/* For. Player Camera */
	CPlayer_CameraPart::PLAYER_CAMERAPART_DESC CameraPartDesc;
	CameraPartDesc.pParent = this;
	CameraPartDesc.pParentTransform = m_pTransformCom;
	// CameraPartDesc.pSocketBone = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketBonePtr("Camera3rd [Cam3]");
	CameraPartDesc.pSocketBone = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketBonePtr("Camera1st [Cam1]");
	CameraPartDesc.matSocketPivot = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketPivotMatrix();

	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Player_CameraPart"), &CameraPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecPlayerPart.push_back(pPart);

	/* For. Weapon */
	CPlayer_Weapon::WEAPON_DESC WeaponPartDesc;
	WeaponPartDesc.pParent = this;
	WeaponPartDesc.pParentTransform = m_pTransformCom;
	WeaponPartDesc.pSocketBone = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketBonePtr("WeaponSword");
	WeaponPartDesc.matSocketPivot = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketPivotMatrix();

	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Player_Weapon"), &WeaponPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecPlayerPart.push_back(pPart);

	/* For. Armor */
	CPlayer_Armor::ARMOR_DESC ArmorPartDesc;
	ArmorPartDesc.pParent = this;
	ArmorPartDesc.pParentTransform = m_pTransformCom; //NPC Spine [Spn0]
	ArmorPartDesc.pSocketBone = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketBonePtr("NPC Spine [Spn0]");
	ArmorPartDesc.matSocketPivot = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketPivotMatrix();
	
	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Player_Armor"), &ArmorPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecPlayerPart.push_back(pPart);
	
	CPlayer_Helmet::HELMET_DESC HelmetPartDesc;
	HelmetPartDesc.pParent = this;
	HelmetPartDesc.pParentTransform = m_pTransformCom; //NPC Spine [Spn0]
	HelmetPartDesc.pSocketBone = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketBonePtr("NPC Spine [Spn0]");
	HelmetPartDesc.matSocketPivot = dynamic_cast<CPlayerPart_Base*>(m_vecPlayerPart[PART_BODY])->Get_SocketPivotMatrix();
	
	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Player_Helmet"), &HelmetPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecPlayerPart.push_back(pPart);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Ready_State()
{
	m_fRunSpeed = 3.5f;
	m_fWalkSpeed = 1.5f;
	m_iHp = 100;
	m_iAtk = 25;

	m_pStateManager = CStateManager_Player::Create(this, m_pTransformCom, m_pNavigationCom[g_curStage]);

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
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pStateManager);

	for (_int i = 0; i < STAGE_END; ++i)
	{
		Safe_Release(m_pNavigationCom[i]);
	}
}
