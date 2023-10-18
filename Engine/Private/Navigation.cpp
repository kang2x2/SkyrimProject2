#include "Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"

_float4x4 CNavigation::m_matWorld = {};

CNavigation::CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_iCurIndex(rhs.m_iCurIndex)
	, m_vecCell(rhs.m_vecCell)
#ifdef _DEBUG
	, m_pShaderCom(rhs.m_pShaderCom)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pShaderCom);
#endif

	for (auto& iter : m_vecCell)
		Safe_AddRef(iter);
}

HRESULT CNavigation::Initialize_ProtoType(const wstring& _strNaviMeshPath)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(_strNaviMeshPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[CCell::POINT_END] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_vecCell.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_vecCell.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOSCELL::Elements, VTXPOSCELL::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize_Clone(void* _pArg)
{
	if (_pArg == nullptr)
		return S_OK;

	DESC_NAVIGATION* pNaviDesc = (DESC_NAVIGATION*)_pArg;

	/*  이 네비게이션을 이용하고자하는 객체가 어떤 셀에 있는지 저장한다. */
	m_iCurIndex = pNaviDesc->iCurIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix _matWorld)
{
	XMStoreFloat4x4(&m_matWorld, _matWorld);

	for (auto& iter : m_vecCell)
	{
		if (iter != nullptr)
			iter->Update(_matWorld);
	}
}

_bool CNavigation::IsMove(_fvector _vPoint)
{
	_int		iNeighborIndex = 0;

	if (true == m_vecCell[m_iCurIndex]->IsOut(_vPoint, XMLoadFloat4x4(&m_matWorld), &iNeighborIndex))
	{
		/* 나간 방향에 이웃셀이 있으면 움직여야해! */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (false == m_vecCell[iNeighborIndex]->IsOut(_vPoint, XMLoadFloat4x4(&m_matWorld), &iNeighborIndex))
				{
					m_iCurIndex = iNeighborIndex;
					break;
				}
			}
			return true;
		}
		else
			return false;

	}
	else
		return true;
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 matView = pPipeLine->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &matView)))
		return E_FAIL;

	_float4x4 matProj = pPipeLine->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &matProj)))
		return E_FAIL;

	Safe_Release(pPipeLine);

	_float4 vColor = {};

	if (-1 == m_iCurIndex)
	{
		vColor = _float4(0.f, 1.f, 0.f, 1.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLineColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		_float		fHeight = 0.f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		for (auto& iter : m_vecCell)
		{
			if (nullptr != iter)
				iter->Render();
		}
	}
	else
	{
		vColor = _float4(1.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLineColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		_float		fHeight = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_vecCell[m_iCurIndex]->Render();
	}

	return S_OK;
}

#endif


HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& sourCell : m_vecCell)
	{
		for (auto& destCell : m_vecCell)
		{
			if (sourCell == destCell)
				continue;

			if (destCell->Compare_Points(sourCell->Get_LocalPoints(CCell::POINT_A), sourCell->Get_LocalPoints(CCell::POINT_B)))
			{
				sourCell->Set_Neighbor(CCell::LINE_AB, destCell);
			}

			else if (destCell->Compare_Points(sourCell->Get_LocalPoints(CCell::POINT_B), sourCell->Get_LocalPoints(CCell::POINT_C)))
			{
				sourCell->Set_Neighbor(CCell::LINE_BC, destCell);
			}

			else if (destCell->Compare_Points(sourCell->Get_LocalPoints(CCell::POINT_C), sourCell->Get_LocalPoints(CCell::POINT_A)))
			{
				sourCell->Set_Neighbor(CCell::LINE_CA, destCell);
			}
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strNaviMeshPath)
{
	CNavigation* pInstance = new CNavigation(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_strNaviMeshPath)))
	{
		MSG_BOX("Fail Create : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* _pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShaderCom);
#endif
}
