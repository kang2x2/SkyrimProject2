#include "Ray_Manager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CRay_Manager)

CRay_Manager::CRay_Manager()
{

}

_float3 CRay_Manager::Return_WorldMousePos(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos) const
{
	// viewport 얻어오기
	UINT viewPortCount = 1;
	D3D11_VIEWPORT vp;
	_pContext->RSGetViewports(&viewPortCount, &vp);

	// viewport -> proj 변환
	_float fX = ((2.f - _WinMousePos.x) / vp.Width) - 1.f;
	_float fY = ((-2.f - _WinMousePos.y) / vp.Height) - 1.f;

	// 투영 행렬 얻어오기
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
	
	// view -> world(추후 충돌 픽킹 시 사용할 광선세팅) 지금은 안 씀
	XMVector3TransformCoord(XMLoadFloat3(&tRay.origin), XMLoadFloat4x4(&matViewInverse));
	XMVector3TransformNormal(XMLoadFloat3(&tRay.direction), XMLoadFloat4x4(&matViewInverse));
	XMVector3Normalize(XMLoadFloat3(&tRay.direction));

	// 광선 없이 임시 허공 피킹(피킹 테스트)
	XMMATRIX matWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&matViewInverse));
	_float3 worldCoord = _float3();
	XMStoreFloat3(&worldCoord, XMVector3TransformCoord(XMVectorSet(fX, fY, 1.f, 1.f), matWorld));



	Safe_Release(pGameInstance);

	return worldCoord;
}

void CRay_Manager::Free()
{
	__super::Free();
}
