#include "Target_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strTargetTag, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vColor)
{
	/* �̹� ������ ��.*/
	if (Find_RenderTarget(_strTargetTag) != nullptr)
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(_pDevice, _pContext, _iSizeX, _iSizeY, _ePixelFormat, _vColor);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	m_mapRenderTarget.emplace(_strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring& _strMRTTag, const wstring& _strTargetTag)
{
	/* �ִ� RenderTarget���� ã��. */
	CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	/* ������ ã�� RenderTarget�� �׷쿡 �߰��ϰڴ�. */
	list<CRenderTarget*>* pltMRT = Find_MRT(_strMRTTag);

	/* ���� ���� �׷��̶�� ���� ���� map�� �߰�. */
	if (pltMRT == nullptr)
	{
		list<CRenderTarget*> ltMRT;
		ltMRT.push_back(pRenderTarget);

		m_mapMRT.emplace(_strMRTTag, ltMRT);
	}
	/* �̹� ������ ã�� list�� �׳� pushback */
	else
	{
		pltMRT->push_back(pRenderTarget);
	}

	/* ���� ����̵� �߰� ������ addref */
	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_SRV(CShader* _pShader, const wstring& _strTargetTag, const _char* _pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	return pRenderTarget->Bind_SRV(_pShader, _pConstantName);
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext* _pContext, const wstring& _strMRTTag)
{
	list<CRenderTarget*>* pltMRT = Find_MRT(_strMRTTag);
	if (pltMRT == nullptr)
		return E_FAIL;

	_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pDSV);

	/* �ִ� 8������ ����. */
	ID3D11RenderTargetView* pRenderTargets[8] = {};

	_uint			iNumRTVs = 0;

	for (auto& pRenderTarget : *pltMRT)
	{
		pRenderTargets[iNumRTVs++] = pRenderTarget->Get_RTV();
		pRenderTarget->Clear();
	}

	_pContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, m_pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* _pContext)
{
	_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDSV);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDSV);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(_fX, _fY, _fSizeX, _fSizeY);
}

HRESULT CTarget_Manager::Render(const wstring& _strMRTTag, CShader* _pShader, CVIBuffer_Rect* _pBuffer)
{
	list<CRenderTarget*>* pltMRT = Find_MRT(_strMRTTag);
	if (pltMRT == nullptr)
		return E_FAIL;

	for (auto& iter : *pltMRT)
	{
		iter->Render(_pShader, _pBuffer);
	}

	return S_OK;
}
#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const wstring& _strTargetTag)
{
	auto iter = m_mapRenderTarget.find(_strTargetTag);

	if (iter == m_mapRenderTarget.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring& _strMRTTag)
{
	auto	iter = m_mapMRT.find(_strMRTTag);

	if (iter == m_mapMRT.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_mapMRT)
	{
		for (auto& iter : Pair.second)
			Safe_Release(iter);

		Pair.second.clear();
	}
	m_mapMRT.clear();


	for (auto& Pair : m_mapRenderTarget)
		Safe_Release(Pair.second);

	m_mapRenderTarget.clear();
}
