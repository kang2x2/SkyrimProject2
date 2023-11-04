#include "Bounding.h"

CBounding::CBounding()
{
}

HRESULT CBounding::Initialize(const BOUNDING_DESC* _pDesc)
{
	return S_OK;
}

void CBounding::Set_OwnerObj(CGameObject* _pObj)
{
	m_pOwnerObj = _pObj;
}

void CBounding::Free()
{
}
