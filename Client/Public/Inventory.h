#pragma once

#include "SkyrimUI.h"

BEGIN(Client)

class CInventory final : public CSkyrimUI
{
public:
	enum INVEN_PART { PART_CATEGORY, PART_LIST, PART_UNDERBAR, PART_END };
private:
	CInventory(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void  Set_IsInvenShow(_bool _bIsShow) { m_bIsShowInven = _bIsShow; }
	_bool Get_IsInvenShow() { return m_bIsShowInven; }

private:
	HRESULT	Ready_Part();
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResources();

private:
	_bool						m_bIsShowInven = false;

	vector<class CGameObject*>  m_vecInvenPart;

public:
	static CInventory* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CSkyrimUI* Clone(void* pArg) override;
	virtual void Free() override;
};

END

