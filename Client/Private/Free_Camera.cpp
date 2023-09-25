#include "framework.h"
#include "Free_Camera.h"

#include "GameInstance.h"

CFree_Camera::CFree_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CFree_Camera::CFree_Camera(const CFree_Camera& rhs)
	: CCamera(rhs)
{
}

HRESULT CFree_Camera::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CFree_Camera::Initialize_Clone(void* pArg)
{
	FREE_CAMERA_DESC* FreeCameraDesc = (FREE_CAMERA_DESC*)pArg;

	m_fMouseSensitive = FreeCameraDesc->fMouseSensitive;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_strName = TEXT("GamePlay_FreeCamera");

	return S_OK;
}

void CFree_Camera::Tick(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 키 입력
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransformCom->Go_Foward(_fTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransformCom->Go_Backward(_fTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Go_Left(_fTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(_fTimeDelta);
	}

	// 마우스 입력
	_long mouseMove = 0l;

	// 일반적인 y축으로 회전 할 것. 
	if (mouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X)) // (y축을 회전하면 x축이 움직이기에.)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), mouseMove * m_fMouseSensitive * _fTimeDelta);
	}

	// 카메라 월드 행렬의 right 축을 기준으로 회전할 것.
	if (mouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), mouseMove * m_fMouseSensitive * _fTimeDelta);
	}

	Safe_Release(pGameInstance);

	__super::Tick(_fTimeDelta);
}

void CFree_Camera::LateTick(_float _fTimeDelta)
{
}

CFree_Camera* CFree_Camera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFree_Camera* pInstance = new CFree_Camera(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CFree_Camera ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFree_Camera::Clone(void* pArg)
{
	CFree_Camera* pInstance = new CFree_Camera(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Clone : CFree_Camera Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFree_Camera::Free()
{
	__super::Free();
}
