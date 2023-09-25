#include "Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

HRESULT CRenderer::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CRenderer::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP _eGroup, CGameObject* _pCloneObject)
{
	// 범위를 벗어나는 잘못된 매개변수를 받았을 때
	if (_eGroup >= RG_END)
		return E_FAIL;

	m_ltRenderObj->push_back(_pCloneObject);

	Safe_AddRef(_pCloneObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderObjects()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

	return S_OK;
}

/* 매 프레임마다 그리고 지우고를 반복. */
HRESULT CRenderer::Render_Priority()
{
	for (auto& iter : m_ltRenderObj[RG_PRIORITY])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& iter : m_ltRenderObj[RG_NONLIGHT])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& iter : m_ltRenderObj[RG_NONBLEND])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_NONBLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& iter : m_ltRenderObj[RG_BLEND])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& iter : m_ltRenderObj[RG_UI])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_UI].clear();

	return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRenderer* pInstance = new CRenderer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();
}
