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

	// Ű �Է�
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

	// ���콺 �Է�
	_long mouseMove = 0l;

	// �Ϲ����� y������ ȸ�� �� ��. 
	if (mouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X)) // (y���� ȸ���ϸ� x���� �����̱⿡.)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), mouseMove * m_fMouseSensitive * _fTimeDelta);
	}

	// ī�޶� ���� ����� right ���� �������� ȸ���� ��.
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
