#pragma once

#include "SkyrimUI.h"

BEGIN(Client)

class CSkyrimUI_Text final : public CSkyrimUI
{
public:
	typedef struct tagTextUIDesc
	{
		_tchar strText[MAX_PATH] = TEXT("");
		
		wstring strFontTag = TEXT("");
		_float	fSizeX = 0.f;
		_float	fSizeY = 0.f;
		_float	fX = 0.f;
		_float	fY = 0.f;
		_vector vColor = {};
	}UI_TEXT_DESC;

private:
	CSkyrimUI_Text(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSkyrimUI_Text(const CSkyrimUI_Text& rhs);
	virtual ~CSkyrimUI_Text() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResources();

private:
	_tchar				m_strText[MAX_PATH] = TEXT("");
	wstring				m_strFontTag = TEXT("");
	_vector				m_vColor = {};

public:
	static CSkyrimUI_Text* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CSkyrimUI* Clone(void* pArg) override;
	virtual void Free() override;
};

END

