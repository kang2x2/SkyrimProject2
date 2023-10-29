#include "Collider.h"

#include "PipeLine.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CCollider::CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_eColliderType(rhs.m_eColliderType)
{
}

HRESULT CCollider::Initialize_ProtoType(COLLIDER_TYPE _eType)
{
	m_eColliderType = _eType;

#ifdef _DEBUG
	/* 디버깅에 대한 준비 */
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);
#endif

	return S_OK;
}

HRESULT CCollider::Initialize_Clone(void* _pArg)
{
	/*  ProtoType 생성 때에는 타입만 정해준 상태. (AABB, OBB 등등) 
		여기에서 사본을 생성할 때에 구조체를 받아
		실제 충돌을 위한 충돌체를 만든다. */

	const CBounding::BOUNDING_DESC* pBoundingDesc =
					(const CBounding::BOUNDING_DESC*)_pArg;

	switch (m_eColliderType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(pBoundingDesc);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(pBoundingDesc);
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(pBoundingDesc);
		break;
	}

	if (m_pBounding == nullptr)
		return E_FAIL;

	return S_OK;
}

void CCollider::Update(_fmatrix _TransformMat)
{
	m_pBounding->Update(_TransformMat);
}

_bool CCollider::IsCollision(CCollider* _pTragetCollider)
{
	return m_pBounding->IsCollision(_pTragetCollider->m_eColliderType, _pTragetCollider->m_pBounding);
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}

#endif

CCollider* CCollider::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COLLIDER_TYPE _eType)
{
	CCollider* pInstance = new CCollider(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_eType)))
	{
		MSG_BOX("Fail Create : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* _pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pBounding);

#ifdef _DEBUG
	if (!m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif
}
