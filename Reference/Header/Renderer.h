#pragma once

#include "Component.h"

// 화면에 그려질 객체들을 그리는 순서대로 그룹별로 관리하면서 랜더콜 수행.

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_NONLIGHT, RG_NONBLEND, RG_BLEND, RG_UI, RG_END };

private:
	CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_ProtoType() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP _eGroup, class CGameObject* _pCloneObject);
	HRESULT Draw_RenderObjects();

private:
	list<class CGameObject*> m_ltRenderObj[RG_END];

private:
	HRESULT Render_Priority();
	HRESULT Render_NonLight();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();

public:
	static CRenderer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

