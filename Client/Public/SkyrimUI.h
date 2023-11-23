#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CTexture;
class CRenderer;
class CShader;
class CVIBuffer_Rect;
class CTransform;

END

BEGIN(Client)

class CSkyrimUI abstract : public CGameObject
{
protected:
	CSkyrimUI(ID3D11Device* _pDeviec, ID3D11DeviceContext* _pContext);
	CSkyrimUI(const CSkyrimUI& rhs);
	virtual ~CSkyrimUI() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

protected: /* 사용할 컴포넌트들 */
	CRenderer*		m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;

protected:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResources();

public:
	/* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual CSkyrimUI* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END

