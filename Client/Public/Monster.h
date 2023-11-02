#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMonster abstract : public CGameObject
{
protected:
	CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual HRESULT Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg); // 사본
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

protected:
	_matrix*		pMatPivot;
	_float4x4		m_matWorld;

public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END

