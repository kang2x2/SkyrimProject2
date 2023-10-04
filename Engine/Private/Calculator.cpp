#include "Calculator.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "VIBuffer_Grid.h"
#include "Transform.h"
#include "Layer.h"
#include "Model.h"
#include "Mesh.h"

using namespace TriangleTests;

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{

}

_float3 CCalculator::Picking_Grid(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
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

			// 교차 지점을 저장할 _float3 변수.
			_float3 vIntersectionPos;

			// 오른쪽 위
			dwVtxIdx[0] = dwIndex + GRIDWIDTH;
			dwVtxIdx[1] = dwIndex + GRIDWIDTH + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
				XMLoadFloat3(&_vec[dwVtxIdx[0]]),
				XMLoadFloat3(&_vec[dwVtxIdx[1]]),
				XMLoadFloat3(&_vec[dwVtxIdx[2]]),
				fDist))
			{
				// 방향 벡터에 거리를 곱하고 시작 지점을 더하여 교차 지점 구한다.
				vIntersectionPos.x = vRayPos.x + vRayDir.x * fDist;
				vIntersectionPos.y = vRayPos.y + vRayDir.y * fDist;
				vIntersectionPos.z = vRayPos.z + vRayDir.z * fDist;

				return vIntersectionPos;
			}

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + GRIDWIDTH;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
				XMLoadFloat3(&_vec[dwVtxIdx[0]]),
				XMLoadFloat3(&_vec[dwVtxIdx[1]]),
				XMLoadFloat3(&_vec[dwVtxIdx[2]]),
				fDist))
			{
				// 방향 벡터에 거리를 곱하고 시작 지점을 더하여 교차 지점 구한다.
				vIntersectionPos.x = vRayPos.x + vRayDir.x * fDist;
				vIntersectionPos.y = vRayPos.y + vRayDir.y * fDist;
				vIntersectionPos.z = vRayPos.z + vRayDir.z * fDist;

				return vIntersectionPos;
			}

		}
	}

	return _float3();
}

_float3 CCalculator::Picking_Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos) const
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iVpNum = 1;
	CD3D11_VIEWPORT vp;

	_pContext->RSGetViewports(&iVpNum, &vp);

	_float4 vCamPosition = pGameInstance->Get_CamPosition_Float4();
	_float4x4 matProjInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ);
	_float4x4 matViewInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	_float3 vMousePos = { 0.f, 0.f, 0.f };

	// 뷰포트 -> 투영
	vMousePos.x = _WinMousePos.x / (vp.Width * 0.5f) - 1.f;
	vMousePos.y = _WinMousePos.y / -(vp.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영 -> 뷰 스페이스
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&matProjInverse)));

	// 광선 구하기
	_float3 vRayPos = { vCamPosition.x, vCamPosition.y, vCamPosition.z };
	_float3 vRayDir;
	XMStoreFloat3(&vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&vRayPos));

	// 뷰 -> 월드
	XMStoreFloat3(&vRayPos, XMVector3TransformCoord(XMLoadFloat3(&vRayPos), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3TransformNormal(XMLoadFloat3(&vRayDir), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));

	// 월드 좌표의 y값이 0이 되는 지점을 찾기 위한 거리 계산
	_float fDist = (0.0f - vRayPos.y) / vRayDir.y;

	// 거리를 사용하여 해당 지점을 계산
	_float3 vIntersectionPoint;
	vIntersectionPoint.x = vRayPos.x + (vRayDir.x * fDist);
	vIntersectionPoint.y = vRayPos.y + (vRayDir.y * fDist);
	vIntersectionPoint.z = vRayPos.z + (vRayDir.z * fDist);

	Safe_Release(pGameInstance);

	return vIntersectionPoint; 
}

CGameObject* CCalculator::Picking_Object(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos, _uint _iLevel)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iVpNum = 1;
	CD3D11_VIEWPORT vp;

	_pContext->RSGetViewports(&iVpNum, &vp);

	_float4 vCamPosition = pGameInstance->Get_CamPosition_Float4();
	_float4x4 matProjInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ);
	_float4x4 matViewInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

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

	// 오브젝트 루프 (오브젝트 매니저로 돌아야 할 듯)
	// 정점 루프 (오브젝트의 정점 정보를 받아와서 돌아야 할 듯)
	// 걸리면 끝 (바로 해당 오브젝트 반환)

	// 해당 레벨에 해당하는 레이어 배열 얻어오기
	map<const wstring, CLayer*>* pLayerMapAry = pGameInstance->Get_CloneObjectMap(_iLevel);

	for (auto Layer = pLayerMapAry->begin(); Layer != pLayerMapAry->end(); ++Layer)
	{
		// Layer 내부의 ObjList를 가져옴.
		list<CGameObject*> objList = Layer->second->Get_ObjList();
		for(auto obj : objList)
		{
			// 여기서 메시가 없는 것들은 거름.
			if (!obj->Get_IsHasMesh())
				continue;

			// 해당 객체의 월드 행렬을 얻어오기 위함.
			CTransform* pObjTransform = dynamic_cast<CTransform*>(obj->Get_Component(TEXT("Com_Transform")));
			_float4x4 matObjWorld = pObjTransform->Get_WorldMatrix_Float4x4();

			// 해당 객체의 모델을 얻어옴.
			CModel* pModel = dynamic_cast<CModel*>(obj->Get_Component(TEXT("Com_Model")));
			
			// 해당 객체의 모델의 메시 벡터를 얻어옴.
			vector<CMesh*> vecMesh = pModel->Get_VecMesh();

			_float fDist = 0.f;

			for (size_t k = 0; k < vecMesh.size(); ++k)
			{
				// 정점과 인덱스 정보를 얻어옴.
				const _float3* pPos = vecMesh[k]->Get_VtxPos();
				const vector<_ulong>& indices = vecMesh[k]->Get_Indices();

				// 메시의 각 삼각형에 대해 교차 검사 수행
				for (size_t idx = 0; idx < indices.size(); idx += 3)
				{
					_float3 vIntersectionPos;

					_ulong idx0 = indices[idx];
					_ulong idx1 = indices[idx + 1];
					_ulong idx2 = indices[idx + 2];

					// 각 삼각형을 구성하는 세 개의 정점을 가져옴
					_float3 vertex0 = pPos[idx0];
					_float3 vertex1 = pPos[idx1];
					_float3 vertex2 = pPos[idx2];

					//// 정점 위치를 월드 공간으로 변환
					XMStoreFloat3(&vertex0, XMVector3TransformCoord(XMLoadFloat3(&vertex0), XMLoadFloat4x4(&matObjWorld)));
					XMStoreFloat3(&vertex1, XMVector3TransformCoord(XMLoadFloat3(&vertex1), XMLoadFloat4x4(&matObjWorld)));
					XMStoreFloat3(&vertex2, XMVector3TransformCoord(XMLoadFloat3(&vertex2), XMLoadFloat4x4(&matObjWorld)));

					if (Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
						XMLoadFloat3(&vertex0),
						XMLoadFloat3(&vertex1),
						XMLoadFloat3(&vertex2),
						fDist))
					{
						// 현재 오브젝트 반환하자.
						Safe_Release(pGameInstance);
						return obj;
					}
				}
			}
		}
	}

	Safe_Release(pGameInstance);

	return nullptr;

}

void CCalculator::Free()
{
	__super::Free();
}
