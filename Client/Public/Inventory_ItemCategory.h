#pragma once

#include "Inventory_PartBase.h"

BEGIN(Client)

class CInventory_ItemCategory final : public CInventory_PartBase
{
private:
	CInventory_ItemCategory(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CInventory_ItemCategory(const CInventory_ItemCategory& rhs);
	virtual ~CInventory_ItemCategory() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual HRESULT Initialize_Clone(void* _pArg);
	virtual void    PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

protected:
	HRESULT	Ready_Component();
	HRESULT Bind_ShaderResources();

public:
	static CInventory_ItemCategory* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

