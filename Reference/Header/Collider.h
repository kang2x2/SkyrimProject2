#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum COLLIDER_TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

private:
	CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_ProtoType(COLLIDER_TYPE _eType);
	virtual HRESULT Initialize_Clone(void* pArg);

public:
	void Update(_fmatrix _TransformMat);

	/* 충돌 체크 */
	_bool IsCollision(CCollider* _pTragetCollider);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	class CBounding*	m_pBounding = nullptr;
	COLLIDER_TYPE		m_eColliderType = TYPE_END;

#ifdef _DEBUG
private:
	/* Vertex, Index Buffer를 생성하고 그리는 기능을 제공하는 객체.  */
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch =  nullptr;
	BasicEffect*								  m_pEffect =  nullptr;
	ID3D11InputLayout*							  m_pInputLayout = nullptr;

#endif

public:
	static CCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COLLIDER_TYPE _eType);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

