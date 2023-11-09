#include "Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint _iLightIndex)
{
	// 예외처리
	if (_iLightIndex >= m_ltLight.size())
		return nullptr;

	// 순회(index까지)
	auto iter = m_ltLight.begin();

	for (size_t i = 0; i < _iLightIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& _LightDesc)
{
	CLight* pLight = CLight::Create(_LightDesc);

	if (pLight == nullptr)
		return E_FAIL;

	m_ltLight.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader* _pShader, CVIBuffer_Rect* _pBuffer)
{
	for (auto& iter : m_ltLight)
		iter->Render(_pShader, _pBuffer);

	return S_OK;
}

void CLight_Manager::Light_Clear()
{
	for (auto& iter : m_ltLight)
		Safe_Release(iter);

	m_ltLight.clear();
}

void CLight_Manager::Free()
{
	__super::Free();

	for (auto& iter : m_ltLight)
		Safe_Release(iter);

	m_ltLight.clear();
}
