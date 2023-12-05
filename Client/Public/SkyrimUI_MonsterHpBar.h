#pragma once

#include "Client_Defines.h"
#include "SkyrimUI.h"

BEGIN(Engine)

class CCreatureObject;
class CTexture;
class CRenderer;
class CShader;
class CVIBuffer_Rect;
class CTransform;

END

BEGIN(Client)

class CSkyrimUI_MonsterHpBar final : public CSkyrimUI
{
private:
	CSkyrimUI_MonsterHpBar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSkyrimUI_MonsterHpBar(const CSkyrimUI_MonsterHpBar& rhs);
	virtual ~CSkyrimUI_MonsterHpBar() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual HRESULT Initialize_Clone(void* _pArg);
	virtual void    PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Monster(CCreatureObject* _pMonster) 
	{
		m_pMonster = _pMonster;
		m_fSizeX = _pMonster->GetHp();
	}

private:
	HRESULT	Ready_Component();
	HRESULT Bind_ShaderResources();

private: /* ����� ������Ʈ�� */
	CTransform* m_pTransformEmpty = nullptr;
	CTexture* m_pTextureEmpty = nullptr;
	CVIBuffer_Rect* m_pVIBufferEmpty = nullptr;
	_float2			m_vEmptyPos = {};
	_float2			m_vEmptySize = {};

	CTransform* m_pTransformFill = nullptr;
	CTexture* m_pTextureFill = nullptr;
	CVIBuffer_Rect* m_pVIBufferFill = nullptr;
	_float2			m_vFillPos = {};
	_float2			m_vFillSize = {};

	CCreatureObject* m_pMonster = nullptr;

public:
	static CSkyrimUI_MonsterHpBar* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CSkyrimUI* Clone(void* _pArg) override;
	virtual void Free()override;
};

END

