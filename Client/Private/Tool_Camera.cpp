#include "framework.h"
#include "Tool_Camera.h"

#include "GameInstance.h"

CTool_Camera::CTool_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CTool_Camera::CTool_Camera(const CTool_Camera& rhs)
	: CCamera(rhs)
{
}

HRESULT CTool_Camera::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CTool_Camera::Initialize_Clone(void* pArg)
{
	TOOL_CAMERA_DESC* FreeCameraDesc = (TOOL_CAMERA_DESC*)pArg;

	m_fMouseSensitive = FreeCameraDesc->fMouseSensitive;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_bHasMesh = false;
	m_strName = TEXT("Tool_Camera");

	return S_OK;
}

void CTool_Camera::Tick(_float _fTimeDelta)
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

	if (pGameInstance->Get_DIKeyState(DIK_P) & 0x8000)
	{
		if(m_bRotMode)
			m_bRotMode = false;
		else
			m_bRotMode = true;
	}

	if (m_bRotMode)
	{
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
	}

	Safe_Release(pGameInstance);

	__super::Tick(_fTimeDelta);
}

void CTool_Camera::LateTick(_float _fTimeDelta)
{
}

CTool_Camera* CTool_Camera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTool_Camera* pInstance = new CTool_Camera(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CTool_Camera ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTool_Camera::Clone(void* pArg)
{
	CTool_Camera* pInstance = new CTool_Camera(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Clone : CTool_Camera Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Camera::Free()
{
	__super::Free();
}
