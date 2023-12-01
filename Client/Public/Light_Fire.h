#pragma once

#include "Skyrim_Light.h"

BEGIN(Engine)

class CTransform;

END

BEGIN(Client)

/* hpp�� �ε��� �߰��ؾ� ��. */
class CLight_Fire final : public CSkyrim_Light
{
private:
	CLight_Fire(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CLight_Fire(const CLight_Fire& rhs);
	virtual ~CLight_Fire() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual HRESULT Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg); // �纻
	virtual void    PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Light();

public:
	static CLight_Fire* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

