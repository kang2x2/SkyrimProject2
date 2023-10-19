#include "Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CCamera::Initialize_Clone(void* pArg)
{
	// ����ü ���� : �Ű� ������ ���� Arg ����.
	CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)pArg;

	// ��� ������ �� ä���(ī�޶� ���� ����)
	vEye    = pCameraDesc->vEye; // ī�޶��� ��ġ
	vAt     = pCameraDesc->vAt;  // ī�޶� �ٶ󺸴� ���� Ȥ�� ���
	fFovY   = pCameraDesc->fFovY;
	fAspect = pCameraDesc->fAspect;
	fNear   = pCameraDesc->fNear;
	fFar    = pCameraDesc->fFar;

	// Ʈ���� �� ����.
	/* ī�޶��� ���¸� �����Ѵ�. */
	/* ������ �׵���·� �����Ѵ�. */
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	/* ������������� �̵�, ȸ�� �ӵ��� ����(pArg �Ű��� �� ���ԵǾ� ����). */
	if (FAILED(m_pTransformCom->Initialize_Clone(pArg)))
		return E_FAIL;

	/* ���� ���ϴ� ī�޶��� ���¸� CTransform ���� ����ȭ�Ѵ�. */
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vEye)); // ��ġ
	m_pTransformCom->LookAt(XMLoadFloat4(&vAt)); // �ٶ� ���� Ȥ�� ���

	matView = m_pTransformCom->Get_WorldMatrix_Inverse();

	return S_OK;
}

void CCamera::Tick(_float _fTimeDelta)
{
	if (m_pPipeLine == nullptr)
		return;
	/* �ڽ�Ŭ������ �ʿ��� ���� ��ȯ�� ���ĳ��� ���´�. */


	/* ���ŵ� ������ ���������ο� ��������. */

	/* ī�޶� ��������� ����� == �佺���̽� ��ȯ���. */
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, matView);
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(fFovY, fAspect, fNear, fFar));
}

void CCamera::LateTick(_float _fTimeDelta)
{
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPipeLine);
}
