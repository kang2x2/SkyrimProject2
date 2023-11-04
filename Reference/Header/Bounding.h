#pragma once

#include "Collider.h"
#include "GameObject.h"

/* AABB, OBB, SPHERE 부모가 되는 클래스다. */

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3 vCenter;
	}BOUNDING_DESC;

protected:
	CBounding();
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize(const BOUNDING_DESC* _pDesc);
	virtual void Update(_fmatrix _TransformMat) = 0;

public:
	virtual _bool IsCollision(CCollider::COLLIDER_TYPE _eType, CBounding* _pBounding) = 0;
	
	virtual CGameObject* Get_OwnerObj() { return m_pOwnerObj; }
	virtual void		 Set_OwnerObj(class CGameObject* _pObj);

protected:
	_bool				m_bIsColl = false;
	CGameObject*		m_pOwnerObj = nullptr;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* _pBatch) = 0;
#endif

public:
	virtual void Free();
};

END

