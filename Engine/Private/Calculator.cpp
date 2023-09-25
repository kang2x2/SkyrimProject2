#include "Calculator.h"

#include "GameInstance.h"

#include "VIBuffer_Grid.h"

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{

}

_float3 CCalculator::Return_WorldMousePos(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos) const
{
	// viewport ������
	UINT viewPortCount = 1;
	D3D11_VIEWPORT vp;
	_pContext->RSGetViewports(&viewPortCount, &vp);

	// viewport -> proj ��ȯ
	_float fX = ((2.f - _WinMousePos.x) / vp.Width) - 1.f;
	_float fY = ((-2.f - _WinMousePos.y) / vp.Height) - 1.f;

	// ���� ��� ������
	_float4x4 matProj;
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	matProj = pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);

	// proj -> view
	fX = fX / matProj.m[0][0];
	fY = fY / matProj.m[1][1];

	RAY tRay;
	ZeroMemory(&tRay, sizeof(RAY));
	tRay.origin = { 0.f, 0.f, 0.f };
	tRay.direction = { fX, fY, 1.f };
	
	_float4x4 matViewInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);
	
	// view -> world(���� �浹 ��ŷ �� ����� ��������) ������ �� ��
	XMVector3TransformCoord(XMLoadFloat3(&tRay.origin), XMLoadFloat4x4(&matViewInverse));
	XMVector3TransformNormal(XMLoadFloat3(&tRay.direction), XMLoadFloat4x4(&matViewInverse));
	XMVector3Normalize(XMLoadFloat3(&tRay.direction));

	// ��ŷ ������ ���� Grid Terrain�� VIBuffer ������ �ʿ��ϴ�.
	CVIBuffer_Grid* pGridBuffer = dynamic_cast<CVIBuffer_Grid*>(pGameInstance->Find_ProtoType(ELEVEL_TOOL, TEXT("ProtoType_Component_VIBuffer_Terrain_Grid")));
	const _float3* pTerrainVtxPos = pGridBuffer->Get_VtxPos();
	// ������ ���ϰ� �ϱ� ���� �迭
	_ulong dwVtxIdx[3]{};

	_float	fU = 0.f, fV = 0.f, fDist = 0.f;

	for (_ulong i = 0; i < GRIDHEIGHT; ++i)
	{
		for (_ulong j = 0; j < GRIDWIDTH; ++j)
		{
			_ulong	iIndex = i * GRIDHEIGHT + j;

			// ������ ��
			dwVtxIdx[0] = iIndex + GRIDHEIGHT;
			dwVtxIdx[1] = iIndex + GRIDHEIGHT + 1;
			dwVtxIdx[2] = iIndex + 1;

			if (Intersects(
				FXMVECTOR(XMLoadFloat3(&tRay.origin)),
				FXMVECTOR(XMLoadFloat3(&tRay.direction)),
				FXMVECTOR(XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[1]])),
				FXMVECTOR(XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[0]])),
				FXMVECTOR(XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[2]])),
				fDist))
			{
				// V1 + U(V2 - V1) + V(V3 - V1)

				return _float3(pTerrainVtxPos[dwVtxIdx[1]].x + fU * (pTerrainVtxPos[dwVtxIdx[0]].x - pTerrainVtxPos[dwVtxIdx[1]].x),
					0.f,
					pTerrainVtxPos[dwVtxIdx[1]].z + fV * (pTerrainVtxPos[dwVtxIdx[2]].z - pTerrainVtxPos[dwVtxIdx[1]].z));
			}

			// ���� �Ʒ�
			dwVtxIdx[0] = iIndex + GRIDWIDTH;
			dwVtxIdx[1] = iIndex + 1;
			dwVtxIdx[2] = iIndex;

			if (Intersects(
				FXMVECTOR(XMLoadFloat3(&tRay.origin)),
				FXMVECTOR(XMLoadFloat3(&tRay.direction)),
				FXMVECTOR(XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[1]])),
				FXMVECTOR(XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[0]])),
				FXMVECTOR(XMLoadFloat3(&pTerrainVtxPos[dwVtxIdx[2]])),
				fDist))
			{
				// V1 + U(V2 - V1) + V(V3 - V1)

				return _float3(pTerrainVtxPos[dwVtxIdx[1]].x + fU * (pTerrainVtxPos[dwVtxIdx[0]].x - pTerrainVtxPos[dwVtxIdx[1]].x),
					0.f,
					pTerrainVtxPos[dwVtxIdx[1]].z + fV * (pTerrainVtxPos[dwVtxIdx[2]].z - pTerrainVtxPos[dwVtxIdx[1]].z));
			}

		}
	}

	Safe_Release(pGameInstance);

	return _float3();
}

void CCalculator::Free()
{
	__super::Free();
}
