#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Zero final : public CLevel
{
private:
	CLevel_Zero(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Zero() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT	Tick(_float _fTimeDelta)override;
	virtual HRESULT	LateTick(_float _fTimeDelta)override;

private:
	void Start_Binary();
	HRESULT Find_FBX(CString _strFolderName);

private:
	_bool		m_bIsBinarying = false;

public:
	static class CLevel_Zero* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
