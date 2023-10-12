#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CBone;
class CModel;
class CRenderer;
class CShader;
class CTransform;

END

BEGIN(Client)

class CPlayer_Body final : public CGameObject
{
public:
	typedef struct tagPartDesc
	{
		CTransform* pParentTransform;
	}PART_DESC;

private:
	CPlayer_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer_Body(const CPlayer_Body& rhs);
	virtual ~CPlayer_Body() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	CBone* Get_SocketBonePtr(const char* _pBoneName);
	_float4x4 Get_SocketPivotMatrix();

public:
	void Set_AnimationIndex(_bool _bIsLoop, _uint _iAnimIndex);

private:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResource();

private:
	CModel*			m_pModelCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;

	CTransform*		m_pParentTransform = nullptr;
	_float4x4		m_WorldMatrix;

public:
	static CPlayer_Body* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

