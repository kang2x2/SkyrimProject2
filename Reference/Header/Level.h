#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual HRESULT	Tick(_float _fTimeDelta);
	virtual HRESULT	LateTick(_float _fTimeDelta);

public:
	virtual void AfterRender();

protected:
	ID3D11Device*		 m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	virtual void Free() override;
};

END

