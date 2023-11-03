#include "framework.h"
#include "SkyrimArmor.h"

CSkyrimArmor::CSkyrimArmor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CSkyrimArmor::CSkyrimArmor(const CSkyrimArmor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyrimArmor::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CSkyrimArmor::Initialize_Clone(void* _pArg)
{
	return S_OK;
}

void CSkyrimArmor::Tick(_float _fTimeDelta)
{
}

void CSkyrimArmor::LateTick(_float _fTimeDelta)
{
}

HRESULT CSkyrimArmor::Render()
{
	return S_OK;
}

void CSkyrimArmor::Free()
{
	__super::Free();
}
