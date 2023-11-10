#include "framework.h"
#include "PlayerCamera.h"

#include "GameInstance.h"

#include "Bone.h"

#include "PlayerPart_Base.h"
#include "Player.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CPlayerCamera::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CPlayerCamera::Initialize_Clone(void* pArg)
{
	FREE_PLAYERCAMERA_DESC* FreeCameraDesc = (FREE_PLAYERCAMERA_DESC*)pArg;

	m_pPlayer	      = FreeCameraDesc->pPlayer;
	m_fMouseSensitive = FreeCameraDesc->fMouseSensitive;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_pPlayerBody = dynamic_cast<CPlayer_Body*>(m_pPlayer->Get_Part(CPlayer::PART_BODY));

	m_bHasMesh = false;
	m_strName = TEXT("GamePlay_FreeCamera");

	Ready_CameraDesc();

	return S_OK;
}

void CPlayerCamera::Tick(_float _fTimeDelta)
{
	Mouse_Fix();
	Zoom(_fTimeDelta);

	if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
	{
		CameraTick_3st(_fTimeDelta);
	}
	else if(m_pPlayer->Get_CamMode() == CPlayer::CAM_1ST)
	{
		CameraTick_1st(_fTimeDelta);
	}

	__super::Tick(_fTimeDelta);
}

void CPlayerCamera::LateTick(_float _fTimeDelta)
{
	if (m_bIsChangeCamMode)
	{
		//if(m_vRelativeCamPos.y > 0.f)
		//	m_vRelativeCamPos.y -= 3.5f * _fTimeDelta;
		//
		//if (m_vRelativeCamPos.z < 0.f)
		//	m_vRelativeCamPos.z += 3.5f * _fTimeDelta;
		//
		//if (m_vRelativeCamPos.y <= 0 && m_vRelativeCamPos.z >= 0)
		//{
			m_pPlayer->Set_PlayerCam(this, CPlayer::CAM_1ST);
			m_pCamBone = m_pPlayerBody->Get_SocketBonePtr("Camera1st [Cam1]");
		//}
	}
}

void CPlayerCamera::CameraTick_1st(_float _fTimeDelta)
{
	/* 내 행렬 * (소캣 뼈의 컴바인드 행렬 * 소캣의 행렬 * 페어런트의 월드 행렬) */
	_float4x4 matSocketCombined = m_pCamBone->Get_CombinedTransformationMatrix();

	_matrix		matSocketWorld = XMLoadFloat4x4(&matSocketCombined) *
		XMLoadFloat4x4(&m_matBodyPivot);

	matSocketWorld.r[0] = XMVector3Normalize(matSocketWorld.r[0]);
	matSocketWorld.r[1] = XMVector3Normalize(matSocketWorld.r[1]);
	matSocketWorld.r[2] = XMVector3Normalize(matSocketWorld.r[2]);

	_float4x4 matSocketWorldFloat4;

	XMStoreFloat4x4(&matSocketWorldFloat4, matSocketWorld);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 마우스 움직임 정도를 얻는다.*/
	_long mouseMoveY = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X);
	_long mouseMoveX = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y);

	_long	MouseMove = 0l;

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * m_fMouseSensitive * _fTimeDelta);
	}

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * m_fMouseSensitive * _fTimeDelta);
	}

	Safe_Release(pGameInstance);

	matSocketWorld = matSocketWorld * m_pPlayerTransform->Get_WorldMatrix();

	_float4x4 matTemp;
	XMStoreFloat4x4(&matTemp, matSocketWorld);

	m_pTransformCom->Set_WorldMatrix(matSocketWorld);

	// m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(matTemp._41 - 0.5f, matTemp._42, matTemp._43, 1.f));

	// m_pPlayerTransform->Set_State(CTransform::STATE_LOOK, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CPlayerCamera::CameraTick_3st(_float _fTimeDelta)
{
	/* 실시간으로 Player 위치 받아온다. */
	XMStoreFloat4(&m_vPlayerPos, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&vEye, XMLoadFloat4(&m_vPlayerPos) + XMVectorSet(m_vRelativeCamPos.x, m_vRelativeCamPos.y, m_vRelativeCamPos.z, 0.f));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 마우스 움직임 정도를 얻는다.*/
	_long mouseMoveY = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X);
	_long mouseMoveX = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y);

	/* 회전 행렬 생성. */
	matRot = XMMatrixRotationY(rotationSpeed * mouseMoveY * _fTimeDelta);
	m_matAccumulateRotY = XMMatrixMultiply(m_matAccumulateRotY, matRot);
	matRot = XMMatrixRotationX(rotationSpeed * mouseMoveX * _fTimeDelta);
	m_matAccumulateRotX = XMMatrixMultiply(m_matAccumulateRotX, matRot);

	/* 위에서 계산한 회전 행렬들을 카메라의 world 행렬과 모두 연산한다.*/
	/*  1. 카메라가 바라볼 중점에 대한 상대 위치 행렬을 설정.
		2. 위에서 구한 마우스에 따른 회전 행렬들을 곱한다.(무조건 x축이 먼저)
		3. 카메라의 위치 행렬을 설정하여 곱한다.
		결론. 카메라의 위치 행렬에서 처음에 정한 상대 위치 만큼 떨어져서 공전이 가능. */
	_matrix matWorld = XMMatrixTranslation(m_vRelativeCamPos.x, m_vRelativeCamPos.y, m_vRelativeCamPos.z);
	matWorld = XMMatrixMultiply(matWorld, m_matAccumulateRotX);
	matWorld = XMMatrixMultiply(matWorld, m_matAccumulateRotY);
	matWorld = XMMatrixMultiply(matWorld, XMMatrixTranslation(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z));
	m_pTransformCom->Set_WorldMatrix(matWorld);

	m_pTransformCom->LookAt(XMLoadFloat4(&m_vPlayerPos));

	if (pGameInstance->Get_DIKeyDown('F'))
	{
		m_bIsChangeCamMode = true;
	}

	Safe_Release(pGameInstance);
}

void CPlayerCamera::Mouse_Fix()
{
	ShowCursor(false);

	POINT		pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

void CPlayerCamera::Zoom(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_long mouseMove = 0l;
	if (mouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_WHEEL))
	{
		if (mouseMove < 0)
			m_pTransformCom->Zoom_Out(_fTimeDelta);
		else if (mouseMove > 0)
			m_pTransformCom->Zoom_In(_fTimeDelta);
	}

	Safe_Release(pGameInstance);
}

void CPlayerCamera::Ready_CameraDesc()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Player"))
			->Get_Component(TEXT("Com_Transform")));

	Safe_Release(pGameInstance);

	m_pPlayer->Set_PlayerCam(this, CPlayer::CAM_3ST);

	m_matBodyPivot = m_pPlayerBody->Get_SocketPivotMatrix();

	m_pCamBone = m_pPlayerBody->Get_SocketBonePtr("Camera3rd [Cam3]");

	m_vRelativeCamPos = { 0.f, 4.f, -3.f };

}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayerCamera* pInstance = new CPlayerCamera(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CPlayerCamera ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerCamera::Clone(void* pArg)
{
	CPlayerCamera* pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Clone : CPlayerCamera Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();
}
