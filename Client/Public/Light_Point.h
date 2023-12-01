#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRenderer;
class CTransform;
class CCollider;

END

BEGIN(Client)

/* hpp�� �ε��� �߰��ؾ� ��. */
class CLight_Point final : public CGameObject
{
private:
	CLight_Point(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CLight_Point(const CLight_Point& rhs);
	virtual ~CLight_Point() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual HRESULT Initialize_LightClone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg, LIGHT_DESC* _LightDesc); // �纻
	virtual void    PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Diffuse(_float4 _vDiffuse);
	void Set_Ambient(_float4 _vAmbient);
	void Set_Specular(_float4 _vSpecular);
	void Set_Direction(_float4 _vDir);
	void Set_Range(_float _fRange);

	LIGHT_DESC* Get_LightDesc();
	void Set_LightDesc(LIGHT_DESC _lightDesc);

	_uint Get_LightIndex() { return m_iLightIndex; }

private:
	HRESULT Ready_Light();
	HRESULT Ready_Light(LIGHT_DESC _pLightDesc);

protected:
	CRenderer*  m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CCollider*  m_pColliderCom = nullptr;

	static _uint g_iLightIndex;
	_uint		m_iLightIndex = 0;

public:
	static  CLight_Point* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual CGameObject* LightClone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg, LIGHT_DESC* _LightDesc) override;
	virtual void Free() override;
};

END

