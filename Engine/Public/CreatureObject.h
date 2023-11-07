#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCreatureObject abstract : public CGameObject
{
protected:
	CCreatureObject(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	CCreatureObject(const CCreatureObject& rhs);
	virtual ~CCreatureObject() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual HRESULT Initialize_Clone(_uint _iLevel, const wstring & _strModelComTag, void* pArg); // �纻
	virtual void    PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	_int	GetHp() { return m_iHp; }
	void	SetHp(_int _iAmount) { m_iHp += _iAmount; }

	_int	GetAtk() { return m_iAtk; }
	void	SetAtk(_int _iAmount) { m_iAtk += _iAmount; }

	_float  GetRunSpeed() { return m_fRunSpeed; }
	_float  GetWalkSpeed() { return m_fWalkSpeed; }

	_bool	GetIsReadyHit() { return m_bIsReadyHit; }
	void    SetIsReadyHit(_bool _bIsHit) { m_bIsReadyHit = _bIsHit; }

protected:
	_int		m_iHp = 0;
	_int		m_iAtk = 0;
	_float		m_fRunSpeed = 0.f;
	_float		m_fWalkSpeed = 0.f;
	_bool		m_bIsReadyHit = false;

public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END

