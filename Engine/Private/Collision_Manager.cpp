#include "Collision_Manager.h"

#include "DebugDraw.h"

#include "GameObject.h"

#include "Collider.h"

#include "Bounding.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

void CCollision_Manager::Collision_AABBTransition(CCollider* _pCollider, CCollider* _pTargetCollider)
{
	/* 내 바운딩과 상대 바운딩과의 출동 처리. */
	_bool m_bIsColl = Is_Collsion(_pCollider, _pTargetCollider);

	if (m_bIsColl)
	{
		int i = 0;
	}
}

_bool CCollision_Manager::Is_Collsion(CCollider* _pCollider, CCollider* _pTargetCollider)
{
	_bool m_bIsColl = false;

	if (_pCollider->Get_ColliderType() == CCollider::TYPE_AABB)
	{
		switch (_pTargetCollider->Get_ColliderType())
		{
		case CCollider::TYPE_AABB:
			m_bIsColl = dynamic_cast<CBounding_AABB*>(_pCollider->Get_Bounding())->Get_Bounding()
				->Intersects(*(dynamic_cast<CBounding_AABB*>(_pTargetCollider)->Get_Bounding()));
			break;
		case CCollider::TYPE_OBB:
			m_bIsColl = dynamic_cast<CBounding_AABB*>(_pCollider->Get_Bounding())->Get_Bounding()
				->Intersects(*(dynamic_cast<CBounding_OBB*>(_pTargetCollider)->Get_Bounding()));
			break;
		case CCollider::TYPE_SPHERE:
			m_bIsColl = dynamic_cast<CBounding_AABB*>(_pCollider->Get_Bounding())->Get_Bounding()
				->Intersects(*(dynamic_cast<CBounding_Sphere*>(_pTargetCollider)->Get_Bounding()));
			break;
		}
	}

	return m_bIsColl;
}

void CCollision_Manager::Free()
{
	__super::Free();
}
