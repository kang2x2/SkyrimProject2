#include "framework.h"
#include "SkyrimWeapon.h"

CSkyrimWeapon::CSkyrimWeapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CSkyrimWeapon::CSkyrimWeapon(const CSkyrimWeapon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyrimWeapon::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CSkyrimWeapon::Initialize_Clone(void* _pArg)
{
	return S_OK;
}

void CSkyrimWeapon::Tick(_float _fTimeDelta)
{
}

void CSkyrimWeapon::LateTick(_float _fTimeDelta)
{
}

HRESULT CSkyrimWeapon::Render()
{
	return S_OK;
}

void CSkyrimWeapon::Free()
{
	__super::Free();
}
