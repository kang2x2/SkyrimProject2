#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Dungeon final : public CLevel
{
private:
	CLevel_Dungeon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Dungeon() = default;

public:
	virtual HRESULT Initialize()override;
	virtual HRESULT	Tick(_float _fTimeDelta)override;
	virtual HRESULT	LateTick(_float _fTimeDelta)override;

public:
	virtual void AfterRender();

private:
	HRESULT Ready_Level();
	HRESULT Ready_Light();

	HRESULT Ready_Layer_Equip(const wstring& _strLayerTag);
	HRESULT Ready_Layer_Player(const wstring& _strLayerTag);
	HRESULT Ready_Layer_Camera(const wstring& _strLayerTag);
	HRESULT Ready_Layer_Navigation_Dungeon(const wstring& _strLayerTag);
	HRESULT Ready_Layer_Sky(const wstring& _strLayerTag);
	HRESULT Ready_Layer_Particle(const wstring& _strLayerTag);

public:
	static CLevel_Dungeon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END

