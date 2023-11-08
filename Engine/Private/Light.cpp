#include "Light.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& _LightDesc)
{
	memmove(&m_LightDesc, &_LightDesc, sizeof(_LightDesc));

	return S_OK;
}

HRESULT CLight::Render(CShader* _pShader, CVIBuffer_Rect* _pBuffer)
{
	if (FAILED(_pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vLightDir, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(_pShader->Begin(1)))
		return E_FAIL;

	if (FAILED(_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& _LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize(_LightDesc)))
	{
		MSG_BOX("Fail Create : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();
}
