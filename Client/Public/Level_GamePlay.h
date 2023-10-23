#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize()override;
	virtual HRESULT	Tick(_float _fTimeDelta)override;
	virtual HRESULT	LateTick(_float _fTimeDelta)override;

public:
	virtual void AfterRender();

private:
	HRESULT Ready_Level();
	HRESULT Ready_Light();
	HRESULT Ready_Layer_Player(const wstring& _strLayerTag);
	HRESULT Ready_Layer_Camera(const wstring& _strLayerTag);
	HRESULT Ready_Layer_Terrain(const wstring& _strLayerTag);

public:
	static CLevel_GamePlay* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END

