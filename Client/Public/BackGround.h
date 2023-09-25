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

class CBackGround final : public CGameObject
{
private:
	CBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBackGround(const CGameObject& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private: /* ����� ������Ʈ�� */
	CRenderer*		 m_pRendererCom = nullptr;
	CShader*		 m_pShaderCom = nullptr;
	CVIBuffer_Rect*  m_pVIBufferCom = nullptr;
	CTexture*		 m_pTextureCom = nullptr;
	CTransform*		 m_pTransformCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResources();

public:
	static CBackGround* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	/* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

