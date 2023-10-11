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

_float3 CCalculator::Picking_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos, CGameObject* _pTerrain, const _float3* _vec, _uint _iLevel)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_bool bIsPickObj = false;
	_uint iVpNum = 1;
	CD3D11_VIEWPORT vp;

	_pContext->RSGetViewports(&iVpNum, &vp);

	_float4 vCamPosition = pGameInstance->Get_CamPosition_Float4();
	_float4x4 matProjInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ);
	_float4x4 matViewInverse = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	_float3 vMousePos = { 0.f, 0.f, 0.f };

	// ����Ʈ -> ����
	vMousePos.x = _WinMousePos.x / (vp.Width * 0.5f) - 1.f;
	vMousePos.y = _WinMousePos.y / -(vp.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// ���� -> �� �����̽�
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&matProjInverse)));

	// ���� ���ϱ�
	_float3 vRayPos = { 0.f, 0.f, 0.f };
	_float3 vRayDir;
	XMStoreFloat3(&vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&vRayPos));

	// �� -> ����
	XMStoreFloat3(&vRayPos, XMVector3TransformCoord(XMLoadFloat3(&vRayPos), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3TransformNormal(XMLoadFloat3(&vRayDir), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));

	// ������Ʈ ���� (������Ʈ �Ŵ����� ���ƾ� �� ��)
	// ���� ���� (������Ʈ�� ���� ������ �޾ƿͼ� ���ƾ� �� ��)
	// �ɸ��� �� (�ٷ� �ش� ������Ʈ ��ȯ)

	// �ش� ������ �ش��ϴ� ���̾� �迭 ������
	map<const wstring, CLayer*>* pLayerMapAry = pGameInstance->Get_CloneObjectMapAry(_iLevel);

	_float		 fDist = 0.f;
	_float       fMinDist = 100000.f; // ī�޶󿡼� ���� ����� �Ÿ�.
	_float3      fProximatePos = { 0.f, 0.f, 0.f }; // ī�޶󿡼� ���� ����� ������Ʈ�� ��ġ.

	for (auto Layer = pLayerMapAry->begin(); Layer != pLayerMapAry->end(); ++Layer)
	{
		// Layer ������ ObjList�� ������.
		list<CGameObject*> objList = Layer->second->Get_ObjList();
		for (auto obj : objList)
		{
			// ���⼭ �޽ð� ���� �͵��� �Ÿ�.
			if (!obj->Get_IsHasMesh())
				continue;
			// Terrain ���̾ �˻�.
			if (obj->Get_ObjFileDesc().m_strLayerTag != TEXT("Layer_Skyrim_WhiteRun_Terrain"))
				continue;

			// �ش� ��ü�� ���� ����� ������ ����.
			CTransform* pObjTransform = dynamic_cast<CTransform*>(obj->Get_Component(TEXT("Com_Transform")));
			_float4x4 matObjWorld = pObjTransform->Get_WorldMatrix_Float4x4();

			// �ش� ��ü�� ���� ����.
			CModel* pModel = dynamic_cast<CModel*>(obj->Get_Component(TEXT("Com_Model")));

			// �ش� ��ü�� ���� �޽� ���͸� ����.
			vector<CMesh*> vecMesh = pModel->Get_VecMesh();

			for (size_t k = 0; k < vecMesh.size(); ++k)
			{
				// ������ �ε��� ������ ����.
				const _float3* pPos = vecMesh[k]->Get_VtxPos();
				const vector<_ulong>& indices = vecMesh[k]->Get_Indices();

				// �޽��� �� �ﰢ���� ���� ���� �˻� ����
				for (size_t idx = 0; idx < indices.size(); idx += 3)
				{
					_float3 vIntersectionPos;

					_ulong idx0 = indices[idx];
					_ulong idx1 = indices[idx + 1];
					_ulong idx2 = indices[idx + 2];

					// �� �ﰢ���� �����ϴ� �� ���� ������ ������
					_float3 vertex0 = pPos[idx0];
					_float3 vertex1 = pPos[idx1];
					_float3 vertex2 = pPos[idx2];

					//// ���� ��ġ�� ���� �������� ��ȯ
					XMStoreFloat3(&vertex0, XMVector3TransformCoord(XMLoadFloat3(&vertex0), XMLoadFloat4x4(&matObjWorld)));
					XMStoreFloat3(&vertex1, XMVector3TransformCoord(XMLoadFloat3(&vertex1), XMLoadFloat4x4(&matObjWorld)));
					XMStoreFloat3(&vertex2, XMVector3TransformCoord(XMLoadFloat3(&vertex2), XMLoadFloat4x4(&matObjWorld)));

					if (Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
						XMLoadFloat3(&vertex0),
						XMLoadFloat3(&vertex1),
						XMLoadFloat3(&vertex2),
						fDist))
					{
						bIsPickObj = true;

						// ���� ���Ϳ� �Ÿ��� ���ϰ� ���� ������ ���Ͽ� ���� ���� ���Ѵ�.
						vIntersectionPos.x = vRayPos.x + vRayDir.x * fDist;
						vIntersectionPos.y = vRayPos.y + vRayDir.y * fDist;
						vIntersectionPos.z = vRayPos.z + vRayDir.z * fDist;

						if (fDist <= fMinDist)
						{
							fMinDist = fDist;
							fProximatePos = vIntersectionPos;
						}
					}
				}
			}
		}
	}

	Safe_Release(pGameInstance);

	if (!bIsPickObj) // ��ŷ�� �ͷ��� ������Ʈ�� ���ٴ� ��. �׸���� �Ѿ��.
		return Picking_Grid(_pDevice, _pContext, _WinMousePos, _pTerrain, _vec);
	else
		return fProximatePos;

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

	// ���� ����� �ͷ����� ���� ����� �ʿ��ϴ�. (������ �󿡼� ���콺 Ŀ���� �ͷ����� ��ġ�� ����?)
	CTransform* pTerrainTransform = dynamic_cast<CTransform*>(_pTerrain->Get_Component(TEXT("Com_Transform")));
	_float4x4 matWorldInverse = pTerrainTransform->Get_WorldMatrix_Float4x4_Inverse();

	_float3 vMousePos = { 0.f, 0.f, 0.f };

	// ����Ʈ -> ����
	vMousePos.x = _WinMousePos.x / (vp.Width * 0.5f) - 1.f;
	vMousePos.y = _WinMousePos.y / -(vp.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// ���� -> �� �����̽�
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&matProjInverse)));

	// ���� ���ϱ�
	_float3 vRayPos = { 0.f, 0.f, 0.f };
	_float3 vRayDir;
	XMStoreFloat3(&vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&vRayPos));

	// �� -> ����
	XMStoreFloat3(&vRayPos, XMVector3TransformCoord(XMLoadFloat3(&vRayPos), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3TransformNormal(XMLoadFloat3(&vRayDir), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));

	// ���� -> ����
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

			// ���� ������ ������ _float3 ����.
			_float3 vIntersectionPos;

			// ������ ��
			dwVtxIdx[0] = dwIndex + GRIDWIDTH;
			dwVtxIdx[1] = dwIndex + GRIDWIDTH + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
				XMLoadFloat3(&_vec[dwVtxIdx[0]]),
				XMLoadFloat3(&_vec[dwVtxIdx[1]]),
				XMLoadFloat3(&_vec[dwVtxIdx[2]]),
				fDist))
			{
				// ���� ���Ϳ� �Ÿ��� ���ϰ� ���� ������ ���Ͽ� ���� ���� ���Ѵ�.
				vIntersectionPos.x = vRayPos.x + vRayDir.x * fDist;
				vIntersectionPos.y = vRayPos.y + vRayDir.y * fDist;
				vIntersectionPos.z = vRayPos.z + vRayDir.z * fDist;

				return vIntersectionPos;
			}

			// ���� �Ʒ�
			dwVtxIdx[0] = dwIndex + GRIDWIDTH;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
				XMLoadFloat3(&_vec[dwVtxIdx[0]]),
				XMLoadFloat3(&_vec[dwVtxIdx[1]]),
				XMLoadFloat3(&_vec[dwVtxIdx[2]]),
				fDist))
			{
				// ���� ���Ϳ� �Ÿ��� ���ϰ� ���� ������ ���Ͽ� ���� ���� ���Ѵ�.
				vIntersectionPos.x = vRayPos.x + vRayDir.x * fDist;
				vIntersectionPos.y = vRayPos.y + vRayDir.y * fDist;
				vIntersectionPos.z = vRayPos.z + vRayDir.z * fDist;

				return vIntersectionPos;
			}
		}
	}

	return _float3();
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

	// ����Ʈ -> ����
	vMousePos.x = _WinMousePos.x / (vp.Width * 0.5f) - 1.f;
	vMousePos.y = _WinMousePos.y / -(vp.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// ���� -> �� �����̽�
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&matProjInverse)));

	// ���� ���ϱ�
	_float3 vRayPos = { 0.f, 0.f, 0.f };
	_float3 vRayDir;
	XMStoreFloat3(&vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&vRayPos));

	// �� -> ����
	XMStoreFloat3(&vRayPos, XMVector3TransformCoord(XMLoadFloat3(&vRayPos), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3TransformNormal(XMLoadFloat3(&vRayDir), XMLoadFloat4x4(&matViewInverse)));
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));

	// ������Ʈ ���� (������Ʈ �Ŵ����� ���ƾ� �� ��)
	// ���� ���� (������Ʈ�� ���� ������ �޾ƿͼ� ���ƾ� �� ��)
	// �ɸ��� �� (�ٷ� �ش� ������Ʈ ��ȯ)

	// �ش� ������ �ش��ϴ� ���̾� �迭 ������
	map<const wstring, CLayer*>* pLayerMapAry = pGameInstance->Get_CloneObjectMapAry(_iLevel);

	_float		 fDist = 0.f;
	_float       fMinDist = 100000.f; // ī�޶󿡼� ���� ����� �Ÿ�.
	CGameObject* pProximateObj = nullptr; // ī�޶󿡼� ���� ����� ������Ʈ.

	_float3 vIntersectionPos;

	for (auto Layer = pLayerMapAry->begin(); Layer != pLayerMapAry->end(); ++Layer)
	{
		// Layer ������ ObjList�� ������.
		list<CGameObject*> objList = Layer->second->Get_ObjList();
		for(auto obj : objList)
		{
			// ���⼭ �޽ð� ���� �͵��� �Ÿ�.
			if (!obj->Get_IsHasMesh())
				continue;

			// �ش� ��ü�� ���� ����� ������ ����.
			CTransform* pObjTransform = dynamic_cast<CTransform*>(obj->Get_Component(TEXT("Com_Transform")));
			_float4x4 matObjWorld = pObjTransform->Get_WorldMatrix_Float4x4();

			// �ش� ��ü�� ���� ����.
			CModel* pModel = dynamic_cast<CModel*>(obj->Get_Component(TEXT("Com_Model")));
			
			// �ش� ��ü�� ���� �޽� ���͸� ����.
			vector<CMesh*> vecMesh = pModel->Get_VecMesh();

			_float fDist = 0.f;

			for (size_t k = 0; k < vecMesh.size(); ++k)
			{
				// ������ �ε��� ������ ����.
				const _float3* pPos = vecMesh[k]->Get_VtxPos();
				const vector<_ulong>& indices = vecMesh[k]->Get_Indices();

				// �޽��� �� �ﰢ���� ���� ���� �˻� ����
				for (size_t idx = 0; idx < indices.size(); idx += 3)
				{

					_ulong idx0 = indices[idx];
					_ulong idx1 = indices[idx + 1];
					_ulong idx2 = indices[idx + 2];

					// �� �ﰢ���� �����ϴ� �� ���� ������ ������
					_float3 vertex0 = pPos[idx0];
					_float3 vertex1 = pPos[idx1];
					_float3 vertex2 = pPos[idx2];

					//// ���� ��ġ�� ���� �������� ��ȯ
					XMStoreFloat3(&vertex0, XMVector3TransformCoord(XMLoadFloat3(&vertex0), XMLoadFloat4x4(&matObjWorld)));
					XMStoreFloat3(&vertex1, XMVector3TransformCoord(XMLoadFloat3(&vertex1), XMLoadFloat4x4(&matObjWorld)));
					XMStoreFloat3(&vertex2, XMVector3TransformCoord(XMLoadFloat3(&vertex2), XMLoadFloat4x4(&matObjWorld)));

					if (Intersects(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir),
						XMLoadFloat3(&vertex0),
						XMLoadFloat3(&vertex1),
						XMLoadFloat3(&vertex2),
						fDist))
					{
						// ���� ���Ϳ� �Ÿ��� ���ϰ� ���� ������ ���Ͽ� ���� ���� ���Ѵ�.
						vIntersectionPos.x = vRayPos.x + vRayDir.x * fDist;
						vIntersectionPos.y = vRayPos.y + vRayDir.y * fDist;
						vIntersectionPos.z = vRayPos.z + vRayDir.z * fDist;

						if (fDist <= fMinDist)
						{
							fMinDist = fDist;
							pProximateObj = obj;
						}
					}
				}
			}
		}
	}

	// cout << vIntersectionPos.x << endl;
	// cout << vIntersectionPos.y << endl;
	// cout << vIntersectionPos.z << endl;

	Safe_Release(pGameInstance);

	if (pProximateObj != nullptr)
		return pProximateObj;
	else
		return nullptr;
}



void CCalculator::Free()
{
	__super::Free();
}
