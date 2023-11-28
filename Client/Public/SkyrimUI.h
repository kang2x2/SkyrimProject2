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
public:
	enum UI_EVENT { UIEVENT_NONE, UIEVENT_CHANGESCENE, UIEVENT_END };
protected:
	CSkyrimUI(ID3D11Device* _pDeviec, ID3D11DeviceContext* _pContext);
	CSkyrimUI(const CSkyrimUI& rhs);
	virtual ~CSkyrimUI() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

protected:
	void UI_InitPos(_float _fX, _float _fY);

protected: /* ����� ������Ʈ�� */
	CRenderer*		m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;

	UI_EVENT		m_eEvent = UIEVENT_NONE;

protected:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResources();

public:
	/* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual CSkyrimUI* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END

