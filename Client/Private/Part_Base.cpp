#include "framework.h"
#include "Part_Base.h"

#include "GameInstance.h"

CPart_Base::CPart_Base(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CPart_Base::CPart_Base(const CPart_Base& rhs)
	: CGameObject(rhs)
{
}

CBone* CPart_Base::Get_SocketBonePtr(const char* _pBoneName)
{
	if (m_pModelCom == nullptr)
		return nullptr;

	return m_pModelCom->Get_BonePtr(_pBoneName);
}

_float4x4 CPart_Base::Get_SocketPivotMatrix()
{
	if (m_pModelCom == nullptr)
		return _float4x4();

	return m_pModelCom->Get_PivotMatrix();
}

HRESULT CPart_Base::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CPart_Base::Initialize_Clone(void* _pArg)
{
	if (_pArg != nullptr)
	{
		PART_DESC* pPartDesc = (PART_DESC*)_pArg;
		m_pParent = pPartDesc->pParent;
		m_pParentTransform = pPartDesc->pParentTransform;
		Safe_AddRef(m_pParentTransform);
	}
	
	return S_OK;
}

void CPart_Base::Tick(_float _fTimeDelta)
{
}

void CPart_Base::LateTick(_float _fTimeDelta)
{
}

HRESULT CPart_Base::Render()
{
	return S_OK;
}

HRESULT CPart_Base::Compute_RenderMatrix(_fmatrix ChildMatrix)
{
	XMStoreFloat4x4(&m_matWorld, ChildMatrix * m_pParentTransform->Get_WorldMatrix());

	return S_OK;
}

CPart_Base* CPart_Base::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return nullptr;
}

CGameObject* CPart_Base::Clone(void* _pArg)
{
	return nullptr;
}

void CPart_Base::Free()
{
	__super::Free();

	// Safe_Release(m_pParentTransform);
}
