#include "framework.h"
#include "Monster.h"

#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	return S_OK;
}

void CMonster::PriorityTick(_float _fTimeDelta)
{
}

void CMonster::Tick(_float _fTimeDelta)
{
}

void CMonster::LateTick(_float _fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Free()
{
	__super::Free();
}
