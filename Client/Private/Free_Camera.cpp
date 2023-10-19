#include "framework.h"
#include "Free_Camera.h"

#include "GameInstance.h"
#include "PipeLine.h"

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

	m_bHasMesh = false;
	m_strName = TEXT("GamePlay_FreeCamera");

	return S_OK;
}

void CFree_Camera::Tick(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Player ��ġ ���ͼ� at�� eye ����. */
	XMStoreFloat4(&PlayerPos, dynamic_cast<CTransform*>
		(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY,
			TEXT("Layer_Player"), TEXT("Player"))
			->Get_Component(TEXT("Com_Transform")))
		->Get_State(CTransform::STATE_POSITION));

	vAt = PlayerPos;
	XMStoreFloat4(&vEye, XMLoadFloat4(&PlayerPos) + XMVectorSet(0.0f, 4.0f, -4.0f, 0.0f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vEye)); 

	/* ���콺 �Է� */
	_long mouseX = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X);
	_long mouseY = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y);

	/* X�� Y ȸ�� ���� ��� �� ���� */ 
	m_fRotationX = (_float)(mouseX) * rotationSpeed * _fTimeDelta;
	m_fRotationY = (_float)(mouseY) * rotationSpeed * _fTimeDelta;

	m_fCurrotationX -= m_fRotationX;
	m_fCurrotationY -= m_fRotationY;

	/* �� ����� ȸ����Ű�� ���� ȸ�� ��� ���� */
	_matrix rotationMatrixX = XMMatrixRotationY(m_fCurrotationX);
	_matrix rotationMatrixY = XMMatrixRotationX(m_fCurrotationY);
	//_matrix rotationMatrixY = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m_fCurrotationY);

	/* �� ����� ������ �� ��Ŀ� ���� ȸ�� ���� */
	_matrix viewMatrix = XMMatrixLookAtLH(XMLoadFloat4(&vEye), XMLoadFloat4(&vAt), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	matView = viewMatrix;
	// rotationMatrixX * rotationMatrixY *  // tlqkf ���߿�
	__super::Tick(_fTimeDelta);


	Safe_Release(pGameInstance);
}

void CFree_Camera::LateTick(_float _fTimeDelta)
{
	m_fRotationX = 0.f;
	m_fRotationY = 0.f;
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
