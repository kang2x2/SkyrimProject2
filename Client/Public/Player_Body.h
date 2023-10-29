#pragma once

#include "Client_Defines.h"
#include "Part_Base.h"

BEGIN(Engine)

class CBone;
class CModel;
class CRenderer;
class CShader;
class CCollider;
class CTransform;

END

BEGIN(Client)

class CPlayer_Body final : public CPart_Base
{
private:
	CPlayer_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer_Body(const CPlayer_Body& rhs);
	virtual ~CPlayer_Body() = default;

public:
	_bool Get_IsAnimationFin();

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_AnimationIndex(_bool _bIsLoop, string _strAnimationName);

private:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResource();

private:
	CCollider* m_pColliderCom = nullptr;

public:
	static CPlayer_Body* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

