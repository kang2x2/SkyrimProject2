#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CSkyrimWeapon abstract : public CGameObject
{
protected:
	CSkyrimWeapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSkyrimWeapon(const CSkyrimWeapon& rhs);
	virtual ~CSkyrimWeapon() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* _pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END

