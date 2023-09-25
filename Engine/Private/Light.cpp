#include "Light.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& _LightDesc)
{
	memmove(&m_LightDesc, &_LightDesc, sizeof(_LightDesc));

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
