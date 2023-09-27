#include "Calculator.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "VIBuffer_Grid.h"
#include "Transform.h"



using namespace TriangleTests;

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{

}

_float3 CCalculator::Return_WorldMousePos(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const POINT& _WinMousePos, CGameObject* _pTerrain, const _float3* _vec) const
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iVpNum = 1;
	CD3D11_VIEWPORT vp;

	_pContext->RSGetViewports(&iVpNum, &vp);

	_float4 vCamPosition = pGameInstance->Get_CamPosition_Float4();
	_float4x4 matProjInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ);
	_float4x4 matViewInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	// 월드 행렬은 터레인의 월드 행렬이 필요하다. (윈도우 상에서 마우스 커서와 터레인이 겹치기 때문?)
	CTransform* pTerrainTransform = dynamic_cast<CTransform*>(_pTerrain->Get_Component(TEXT("Com_Transform")));
	_float4x4 matWorldInverse = pTerrainTransform->Get_WorldMatrix_Float4x4_Inverse();

	_float3 vMousePos = { 0.f, 0.f, 0.f };

	// 뷰포트 -> 투영
	vMousePos.x = _WinMousePos.x / (vp.Width * 0.5f) - 1.f;
	vMousePos.y = _WinMousePos.y / -(vp.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영 -> 뷰 스페이스
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&matProjInverse)));

	// 광선 구하기
	_float3 vRayPos = { 0.f, 0.f, 0.f };
	_float3 vRayDir;
	XMStoreFloat3(&vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&vRayPos));

	// 뷰 -> 월드
	XMStoreFloat3(&vRayPos, XMVector3TransformCoord(XMLoadFloat3(&vRayPos), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3TransformNormal(XMLoadFloat3(&vRayDir), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));

	// 월드 -> 로컬
	XMStoreFloat3(&vRayPos, XMVector3TransformCoord(XMLoadFloat3(&vRayPos), XMLoadFloat4x4(&matWorldInverse)));
	XMStoreFloat3(&vRayDir, XMVector3TransformNormal(XMLoadFloat3(&vRayDir), XMLoadFloat4x4(&matWorldInverse)));
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));

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

			if (true == Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
				XMLoadFloat3(&_vec[dwVtxIdx[0]]),
				XMLoadFloat3(&_vec[dwVtxIdx[1]]),
				XMLoadFloat3(&_vec[dwVtxIdx[2]]),
				fDist))
			{
				return _vec[dwVtxIdx[1]];
			}

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + GRIDWIDTH;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (true == Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
				XMLoadFloat3(&_vec[dwVtxIdx[0]]),
				XMLoadFloat3(&_vec[dwVtxIdx[1]]),
				XMLoadFloat3(&_vec[dwVtxIdx[2]]),
				fDist))
			{
				return _vec[dwVtxIdx[2]];
			}

		}
	}

	return _float3();

}

void CCalculator::Free()
{
	__super::Free();
}
