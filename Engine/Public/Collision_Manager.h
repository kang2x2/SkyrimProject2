#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	void Collision_AABBTransition(class CCollider* _pCollider, class CCollider* _pTargetCollider);


private:
	_bool Is_Collsion(class CCollider* _pCollider, class CCollider* _pTargetCollider);


public:
	virtual void Free() override;
};

END
