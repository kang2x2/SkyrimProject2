#pragma once

#include "Collider.h"

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
	
public:
	virtual void Set_bISCol(_bool _bIsCol) { m_bIsCol = _bIsCol; }

protected:
	_bool				m_bIsCol = false;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* _pBatch) = 0;
#endif

public:
	virtual void Free();
};

END

