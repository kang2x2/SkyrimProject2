#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint _iLightIndex);

	HRESULT Add_Light(const LIGHT_DESC& _LightDesc);
	HRESULT Render(class CShader* _pShader, class CVIBuffer_Rect* _pBuffer);

private:
	list<class CLight*> m_ltLight;

public:
	virtual void Free() override;
};

END

