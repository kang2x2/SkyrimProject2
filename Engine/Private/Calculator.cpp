#include "Calculator.h"

#include "GameInstance.h"

#include "VIBuffer_Grid.h"

using namespace TriangleTests;

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{

}

_float3 CCalculator::Return_WorldMousePos(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos) const
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iVpNum = 1;
	CD3D11_VIEWPORT vp;

	_pContext->RSGetViewports(&iVpNum, &vp);

	_float3 vRay;

	_float4 vCamPosition = pGameInstance->Get_CamPosition_Float4();
	_float4x4 matProj = pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);
	_float4x4 matViewInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	vRay.x = (((2.f * _WinMousePos.x) / vp.Width) - 1.f) / matProj.m[0][0];
	vRay.y = (((-2.f * _WinMousePos.y) / vp.Height) + 1.f) / matProj.m[1][1];
	vRay.z = 1.f;

	_float4 vOrigin = { vCamPosition.x, vCamPosition.y, vCamPosition.z, vCamPosition.w };
	_float4 vDir = { vRay.x, vRay.y, vRay.z, 0.f};

	XMStoreFloat4(&vOrigin, XMVector3TransformCoord(XMLoadFloat4(&vOrigin), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat4(&vDir, XMVector3TransformNormal(XMLoadFloat4(&vDir), XMLoadFloat4x4(&matViewInverse)));

	XMStoreFloat4(&vDir, XMVector3Normalize(XMLoadFloat4(&vDir)));

	CVIBuffer_Grid* pGridBuffer = dynamic_cast<CVIBuffer_Grid*>(pGameInstance->Find_ProtoType(ELEVEL_TOOL, TEXT("ProtoType_Component_VIBuffer_Terrain_Grid")));
	const _float3* pTerrainVtxPos = pGridBuffer->Get_VtxPos();

	Safe_Release(pGameInstance);

	_ulong		dwVtxIdx[3]{};
	_float		fDist = 0.f;

	for (_ulong i = 0; i < GRIDHEIGHT - 1; ++i)
	{
		for (_ulong j = 0; j < GRIDWIDTH - 1; ++j)
		{
			_ulong	dwIndex = i * GRIDWIDTH + j;

			// 오른쪽 위
			dwVtxIdx[0] = dwIndex + GRIDWIDTH;
			dwVtxIdx[1] = dwIndex + GRIDWIDTH + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (Intersects(XMLoadFloat4(&vOrigin), XMLoadFloat4(&vDir),
				XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[1]]),
				XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[0]]),
				XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[2]]),
				fDist))
			{
				return pTerrainVtxPos[dwVtxIdx[0]];
			}

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + GRIDWIDTH;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (Intersects(XMLoadFloat4(&vOrigin), XMLoadFloat4(&vDir),
				XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[1]]),
				XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[0]]),
				XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[2]]),
				fDist))
			{
				return pTerrainVtxPos[dwVtxIdx[0]];
			}

		}
	}

	return _float3();

}

void CCalculator::Free()
{
	__super::Free();
}
