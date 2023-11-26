#pragma once

#include "Inventory_PartBase.h"

BEGIN(Client)

class CInventory_ItemCategory final : public CInventory_PartBase
{
public:
	enum INVEN_CATEGORY { CATEGORY_FAVORITES, CATEGORY_ALL, CATEGORY_WEAPON, CATEGORY_ARMOR, CATEGORY_POTION, CATEGORY_FOOD, CATEGORY_END };

	typedef struct tagInvenCategoryDesc
	{
		wstring strText = TEXT("");
		_float2 vPos = {};
		_vector vColor = { 0.2f, 0.2f, 0.2f };
		RECT    rc = {};
	}CATEGORY_DESC;

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

public:
	HRESULT	Ready_Component();
	void	Ready_CategoryFont();
	HRESULT Bind_ShaderResources();

private:
	vector<CATEGORY_DESC> m_vecCategory;

public:
	static CInventory_ItemCategory* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

