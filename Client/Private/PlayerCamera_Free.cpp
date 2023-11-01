#include "framework.h"
#include "PlayerCamera_Free.h"

#include "GameInstance.h"

CPlayerCamera_Free::CPlayerCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CPlayerCamera_Free::CPlayerCamera_Free(const CPlayerCamera_Free& rhs)
	: CCamera(rhs)
{
}

HRESULT CPlayerCamera_Free::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CPlayerCamera_Free::Initialize_Clone(void* pArg)
{
	FREE_PLAYERCAMERA_DESC* FreeCameraDesc = (FREE_PLAYERCAMERA_DESC*)pArg;

	m_pPlayer	      = FreeCameraDesc->pPlayer;
	m_fMouseSensitive = FreeCameraDesc->fMouseSensitive;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_bHasMesh = false;
	m_strName = TEXT("GamePlay_FreeCamera");

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(pGameInstance->Find_CloneObject(LEVEL_WHITERUN,
			TEXT("Layer_Player"), TEXT("Player"))
			->Get_Component(TEXT("Com_Transform")));

	Safe_Release(pGameInstance);

	m_pPlayer->Set_PlayerCam(this, CPlayer::CAM_FREE);

	m_vRelativeCamPos = { 0.f, 4.f, -3.f };

	return S_OK;
}

void CPlayerCamera_Free::Tick(_float _fTimeDelta)
{
	Zoom(_fTimeDelta);

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

	Safe_Release(pGameInstance);

	__super::Tick(_fTimeDelta);
}

void CPlayerCamera_Free::LateTick(_float _fTimeDelta)
{
}

void CPlayerCamera_Free::Zoom(_float _fTimeDelta)
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

CPlayerCamera_Free* CPlayerCamera_Free::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayerCamera_Free* pInstance = new CPlayerCamera_Free(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CPlayerCamera_Free ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerCamera_Free::Clone(void* pArg)
{
	CPlayerCamera_Free* pInstance = new CPlayerCamera_Free(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Clone : CPlayerCamera_Free Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera_Free::Free()
{
	__super::Free();
}
