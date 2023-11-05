#include "framework.h"
#include "Loader.h"

#include "GameInstance.h"

#include "Collider.h"

// Logo Level
#include "BackGround.h"

// GamePlay Level
#include "PlayerCamera_Free.h"
#include "Navigation_Client.h"
#include "Terrain.h"
#include "Sky.h"
#include "ParticleRect.h"

#include "Player.h"
#include "Player_Body.h"
#include "Player_Weapon.h"
#include "Player_Armor.h"
#include "Player_Helmet.h"

// Tool Level
#include "Tool_Camera.h"

#include "Terrain_Grid.h"

#include "SkyrimTerrain.h"
#include "Building.h"
#include "StoneWork.h"

/* Monster */
#include "Skeever.h"
#include "Falmer_UnEquip.h"

/* Equip */
#include "Weapon_IronSword.h"
#include "Armor_Glass.h"
#include "Helmet_Glass.h"

CLoader::CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint __stdcall ThreadEntry(void* pArg)
{
	/* DX가 사용하는 모든 컴 객체를 초기화한다.  */
	CoInitializeEx(nullptr, 0);

	/* 새롭게 생성된 스레드가 일한다. */
	CLoader* pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader::Initialize(LEVELID _eNextLevel)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = _eNextLevel;

	// 새로운 스레드 생성
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);

	if (m_hThread == 0)
		return E_FAIL;

	return S_OK;
}

_int CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT hr = 0;

	switch (m_eNextLevel)
	{
	case LEVEL_ZERO:
		hr = Loading_For_Level_Zero();
		break;
	case LEVEL_TOOL:
		hr = Loading_For_Level_Tool();
		break;
	case LEVEL_LOGO:
		hr = Loading_For_Level_Logo();
		break;
	case LEVEL_WHITERUN:
		hr = Loading_For_Level_WhiteRun();
		break;
	case LEVEL_DUNGEON:
		hr = Loading_For_Level_Dungeon();
		break;

	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&m_Critical_Section);

	return 0;
}

HRESULT CLoader::Loading_For_Level_Zero()
{
	//m_strLoadingText = TEXT("Loading Mesh.");
	//
	//Set_ProtoType_Mesh(LEVEL_ZERO);

	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;
	
	g_curLevel = LEVEL_ZERO;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Tool()
{
 	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Texture */
	m_strLoadingText = TEXT("Loading Texture.");

	/* Shader */
	m_strLoadingText = TEXT("Loading Shader.");

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_TOOL, TEXT("ProtoType_Component_Shader_VtxPosCol"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxPosCol.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNonAnimMesh.hlsl"), VTX_NONANIMMESH::Elements, VTX_NONANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxAnimMesh.hlsl"), VTX_ANIMMESH::Elements, VTX_ANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.ProtoType_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.ProtoType_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* Mesh*/
	m_strLoadingText = TEXT("Loading Mesh.");
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_TOOL, TEXT("ProtoType_Component_VIBuffer_Terrain_Grid"),
		CVIBuffer_Grid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Set_ProtoType_WhiteRunMesh(LEVEL_TOOL);

	/* 일단 화이트런에 통합되어 있음.*/
	// Set_ProtoType_DungeonMesh(LEVEL_TOOL);

	_matrix matInitialize = XMMatrixIdentity();

	matInitialize = XMMatrixScaling(0.0013f, 0.0013f, 0.0013f);
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_TOOL, TEXT("ProtoType_Component_Model_Player_Body"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/Anim/Skyrim_Player/Player_1Hand_Stand.bin", matInitialize, CModel::TYPE_ANIM))))
		return E_FAIL;


#pragma region GameObject
	// Camera
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_ToolCamera"),
		CTool_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	// Grid Terrain
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GridTerrain"),
		CTerrain_Grid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Player
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player_Body"),
		CPlayer_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	Set_ProtoType_WhiteObject();
	/* 일단 화이트런에 통합되어 있음.*/
	// Set_ProtoType_DungeonObject();

#pragma endregion

	/* 로딩 끝 */
	Safe_Release(pGameInstance);

	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;

	g_curLevel = LEVEL_TOOL;

	return S_OK;

}

HRESULT CLoader::Loading_For_Level_Logo()
{
	CGameInstance* pGameInstanc = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstanc);

	/* Texture */
	m_strLoadingText = TEXT("Loading Texture.");
	if(FAILED(pGameInstanc->Add_ProtoType_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/LogoBackGround.dds"), 1))))
		return E_FAIL;

	/* Mesh */
	m_strLoadingText = TEXT("Loading Mesh.");

	/* Shader */
	m_strLoadingText = TEXT("Loading Shader.");

	/* GameObject(원본) */
	m_strLoadingText = TEXT("Loading ProtoType_GameObject.");
	// BackGround_ProtoType
	if (FAILED(pGameInstanc->Add_ProtoObject(TEXT("ProtoType_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* 로딩 끝 */
	Safe_Release(pGameInstanc);
	
	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;

	g_curLevel = LEVEL_LOGO;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_WhiteRun()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region Texture
	m_strLoadingText = TEXT("Loading Texture.");

	// Terrain 
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Terrain_Mask */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* Brush */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Texture_Terrain_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* Sky */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* Snow */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Snow/sun.dds"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region Collider

	m_strLoadingText = TEXT("Loading Collider.");

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.ProtoType_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;


#pragma endregion

#pragma region Mesh
	/* Mesh */
	m_strLoadingText = TEXT("Loading Mesh.");
	_matrix matInitialize = XMMatrixIdentity();

	/* Terrain */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* VIBuffer_Rect_Instance */

	CVIBuffer_Rect_Instance::INSTANCE_DESC InstanceDesc = {};

	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vRange = _float3(0.1f, 0.f, 0.1f);
	InstanceDesc.fScaleMin = 0.01f;
	InstanceDesc.fScaleMax = 0.04f;
	InstanceDesc.iNumInstance = 20;
	InstanceDesc.fLifeTimeMin = 0.5f;
	InstanceDesc.fLifeTimeMax = 2.0f;
	InstanceDesc.fSpeedMin = 0.1f;
	InstanceDesc.fSpeedMax = 0.5f;

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;

	/* Player */
	//  * XMMatrixTranslation(1.f, -1.3f, 12.f)
	matInitialize = XMMatrixScaling(0.0013f, 0.0013f, 0.0013f);
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Model_Player_Body"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/Anim/Skyrim_Player/Player_1Hand_Stand.bin", matInitialize, CModel::TYPE_ANIM))))
		return E_FAIL;

	/* Item */
	matInitialize = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Model_Weapon_IronSword"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Weapon/1Hand/IronSword/Iron_LongSword.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Model_Glass_Boots"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Armor/Glass/Glass_Boots/Glass_Boots.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	// matInitialize = XMMatrixRotationX(XMConvertToRadians(-10.f));
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Model_Glass_Curiass"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Armor/Glass/Glass_Curiass/Glass_Curiass.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	matInitialize = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Model_Glass_Gauntlet"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Armor/Glass/Glass_Gauntlet/Glass_Gauntlet.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Model_Glass_Helmet"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Armor/Glass/Glass_Helmet/Glass_Helmet.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	/* Skeever */
	//matInitialize = XMMatrixIdentity();
	//matInitialize = XMMatrixScaling(0.0013f, 0.0013f, 0.0013f);
	//if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Model_Skeever"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/Anim/Skyrim_Skeever/Skeever.bin", matInitialize, CModel::TYPE_ANIM))))
	//	return E_FAIL;

	matInitialize = XMMatrixIdentity();

	Set_ProtoType_WhiteRunMesh(LEVEL_WHITERUN);

#pragma endregion

#pragma region Shader
	/* Shader */
	m_strLoadingText = TEXT("Loading Shader.");
	
	/* Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxAnimMesh.hlsl"), VTX_ANIMMESH::Elements, VTX_ANIMMESH::iNumElements))))
		return E_FAIL;
	
	/* Shader_VtxNonAnimMesh */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNonAnimMesh.hlsl"), VTX_NONANIMMESH::Elements, VTX_NONANIMMESH::iNumElements))))
		return E_FAIL;

	/* Shader_VtxCube */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* Shader_Rect_Instance */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_Rect_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_Rect_Instance.hlsl"), VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region GameObject
	/* GameObject */
	m_strLoadingText = TEXT("Loading ProtoType_GameObject.");

	// Camera
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_FreePlayerCamera"),
		CPlayerCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Sky
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Particle
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Particle_Rect"),
		CParticleRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Terrain
	//if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Terrain"),
	//	CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	// Player
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player_Body"),
		CPlayer_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player_Weapon"),
		CPlayer_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player_Armor"),
		CPlayer_Armor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player_Helmet"),
		CPlayer_Armor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	// Item
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Weapon_IronSword"),
		CWeapon_IronSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Glass_Boots"),
	//	CWeapon_IronSword::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Glass_Curiass"),
		CArmor_Glass::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//
	//if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Glass_Gauntlet"),
	//	CWeapon_IronSword::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Glass_Helmet"),
		CHelmet_Glass::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Set_ProtoType_WhiteObject();

#pragma endregion

#pragma region Navigation
	m_strLoadingText = TEXT("Loading ProtoType_Navigation.");

	/* 화이트런 */
	//if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/SaveLoad/Celltest")))))
	//	return E_FAIL;

	/* 던전 임시 확인*/
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_WHITERUN, TEXT("ProtoType_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/SaveLoad/Dungeon_Cell")))))
		return E_FAIL;

	// Navigation Client
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Navigation_WhiteRun"),
		CNavigation_Client::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	Safe_Release(pGameInstance);

	/* 로딩 끝 */
	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;

	g_curLevel = LEVEL_WHITERUN;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Dungeon()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region Texture
	m_strLoadingText = TEXT("Loading Texture.");

	/* Sky */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* Snow */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Snow/sun.dds"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region Collider

	m_strLoadingText = TEXT("Loading Collider.");

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;


#pragma endregion

#pragma region Mesh
	/* Mesh */
	m_strLoadingText = TEXT("Loading Mesh.");
	_matrix matInitialize = XMMatrixIdentity();

	/* VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* VIBuffer_Rect_Instance */

	CVIBuffer_Rect_Instance::INSTANCE_DESC InstanceDesc = {};

	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vRange = _float3(0.1f, 0.f, 0.1f);
	InstanceDesc.fScaleMin = 0.01f;
	InstanceDesc.fScaleMax = 0.04f;
	InstanceDesc.iNumInstance = 20;
	InstanceDesc.fLifeTimeMin = 0.5f;
	InstanceDesc.fLifeTimeMax = 2.0f;
	InstanceDesc.fSpeedMin = 0.1f;
	InstanceDesc.fSpeedMax = 0.5f;

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;

	/* Player */
	//  * XMMatrixTranslation(1.f, -1.3f, 12.f)
	matInitialize = XMMatrixScaling(0.0013f, 0.0013f, 0.0013f);
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Model_Player_Body"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/Anim/Skyrim_Player/Player_1Hand_Stand.bin", matInitialize, CModel::TYPE_ANIM))))
		return E_FAIL;

	/* Weapon */
	matInitialize = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Model_Weapon_IronSword"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_Weapon/1Hand/IronSword/Iron_LongSword.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	Set_ProtoType_DungeonMesh(LEVEL_DUNGEON);

#pragma endregion

#pragma region Shader
	/* Shader */
	m_strLoadingText = TEXT("Loading Shader.");

	/* Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxAnimMesh.hlsl"), VTX_ANIMMESH::Elements, VTX_ANIMMESH::iNumElements))))
		return E_FAIL;

	/* Shader_VtxNonAnimMesh */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxNonAnimMesh.hlsl"), VTX_NONANIMMESH::Elements, VTX_NONANIMMESH::iNumElements))))
		return E_FAIL;

	/* Shader_VtxCube */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* Shader_Rect_Instance */
	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Shader_Rect_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_Rect_Instance.hlsl"), VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region GameObject
	/* GameObject */
	m_strLoadingText = TEXT("Loading ProtoType_GameObject.");

#pragma region 나중에 지울 것들. 차피 WhiteRun에서 이미 생성했다.
	// Camera
	 if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_FreePlayerCamera"),
	 	CPlayerCamera_Free::Create(m_pDevice, m_pContext))))
	 	return E_FAIL;
	 
	// Sky
	//if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Sky"),
	//	CSky::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	// Particle
	//if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Particle_Rect"),
	//	CParticleRect::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	// Terrain
	//if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Terrain"),
	//	CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	// 
	// if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player"),
	// 	CPlayer::Create(m_pDevice, m_pContext))))
	// 	return E_FAIL;
	// if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Player_Body"),
	// 	CPlayer_Body::Create(m_pDevice, m_pContext))))
	// 	return E_FAIL;

	// Weapon
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Weapon_IronSword"),
	 	CWeapon_IronSword::Create(m_pDevice, m_pContext))))
	 	return E_FAIL;

#pragma endregion

	Set_ProtoType_DungeonObject();

#pragma endregion

#pragma region Navigation
	m_strLoadingText = TEXT("Loading ProtoType_Navigation.");

	if (FAILED(pGameInstance->Add_ProtoType_Component(LEVEL_DUNGEON, TEXT("ProtoType_Component_Navigation_Dungeon"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/SaveLoad/Dungeon_Cell")))))
		return E_FAIL;

	// Navigation 
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Navigation_Dungeon"),
		CNavigation_Client::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	Safe_Release(pGameInstance);

	/* 로딩 끝 */
	m_strLoadingText = TEXT("Loading Complete");
	m_bIsFinish = true;

	g_curLevel = LEVEL_DUNGEON;

	return S_OK;
}

HRESULT CLoader::Set_ProtoType_WhiteRunMesh(LEVELID _eLevel)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_matrix matInitialize = XMMatrixIdentity();
	matInitialize = XMMatrixScaling(0.0013f, 0.0013f, 0.0013f);

	/* 임시 */
	/* Skeever */
	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Skeever"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/Anim/Skyrim_Skeever/Skeever.bin", matInitialize, CModel::TYPE_ANIM))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Skeever"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/Anim/Skyrim_Skeever/Skeever.bin", matInitialize, CModel::TYPE_ANIM))))
		return E_FAIL;

	matInitialize = XMMatrixIdentity();
	matInitialize = XMMatrixScaling(0.0013f, 0.0013f, 0.0013f);

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Falmer_Unequip"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/Anim/Skyrim_Falmer_UnEquip/Falmer_Unequip.bin", matInitialize, CModel::TYPE_ANIM))))
		return E_FAIL;

	matInitialize = XMMatrixIdentity();
	matInitialize = XMMatrixScaling(0.01f, 0.01f, 0.01f);

#pragma region SkyrimTerrain
	/* Terrain */
 	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ArcadiaBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/ArcadiaBase01/ArcadiaBase01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BanneredMareBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/BanneredMareBase01/BanneredMareBase01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BlackSmithBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/BlackSmithBase/BlackSmithBase.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BreezeBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/BreezeBase/BreezeBase.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BridgeStone01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/BridgeStone01/BridgeStone01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CarlottaPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CarlottaPlatform01/CarlottaPlatform01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CastleStairs01/CastleStairs01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CloudDistrictTerrainlod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CloudDistrictTerrainlod/CloudDistrictTerrainlod.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CommonHousePlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CommonHousePlatform01/CommonHousePlatform01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CompanionBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/CompanionBase01/CompanionBase01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DrunkenHuntsmanBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/DrunkenHuntsmanBase/DrunkenHuntsmanBase.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DryGoodsBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/DryGoodsBase/DryGoodsBase.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GreatHousePlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/GreatHousePlatform01/GreatHousePlatform01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GreatHousePlatform02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/GreatHousePlatform02/GreatHousePlatform02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GreymaneBase"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/GreymaneBase/GreymaneBase.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GuardHouse01Arch01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/GuardHouse01Arch01/GuardHouse01Arch01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HalloFdeadPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/HalloFdeadPlatform01/HalloFdeadPlatform01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadMarket"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/MainRoadMarket/MainRoadMarket.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadPlains01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/MainRoadPlains01/MainRoadPlains01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MainRoadPlains02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/MainRoadPlains02/MainRoadPlains02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PlainsDistrictTerrainlod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PlainsDistrictTerrainlod/PlainsDistrictTerrainlod.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondStairs01/PondStairs01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondStairs02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondStairs02/PondStairs02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallCor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondWallCor01/PondWallCor01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondWallStr01/PondWallStr01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondWallStr02/PondWallStr02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_PondWallStr03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/PondWallStr03/PondWallStr03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ShacksBase01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/ShacksBase01/ShacksBase01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_ShacksBase02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/ShacksBase02/ShacksBase02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/SkyForge01/SkyForge01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01Door01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/SkyForge01Door01/SkyForge01Door01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_SkyForge01lod"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/SkyForge01lod/SkyForge01lod.bin", matInitialize))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StairsPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/StairsPlatform01/StairsPlatform01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StairsStreamExit01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/StairsStreamExit01/StairsStreamExit01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TerCloud01/TerCloud01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TerCloud02/TerCloud02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerCloud03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TerCloud03/TerCloud03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TerWind01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TerWind01/TerWind01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TreeCircle01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TreeCircle01/TreeCircle01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TreeCircleWater01"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/TreeCircleWater01/TreeCircleWater01.bin", matInitialize, CModel::TYPE_NONANIM))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_UlfberhPlatform01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/UlfberhPlatform01/UlfberhPlatform01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WaterPool01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/WaterPool01/WaterPool01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WindDistrictTerrainLod"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Terrain/WindDistrictTerrainLod/WindDistrictTerrainLod.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region Building
	/* Building */
	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_BreezeHome"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/BreezeHome/BreezeHome.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Bridge01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Bridge01/Bridge01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Bridge02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Bridge02/Bridge02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleBridge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/CastleBridge/CastleBridge.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleEntrance01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/CastleEntrance01/CastleEntrance01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleMainBuilding01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/CastleMainBuilding01/CastleMainBuilding01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleStoneTower01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/CastleStoneTower01/CastleStoneTower01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleStoneTower01Porch"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/CastleStoneTower01Porch/CastleStoneTower01Porch.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleStoneTower01Porchint"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/CastleStoneTower01Porchint/CastleStoneTower01Porchint.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_CastleStoneTowerTop01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/CastleStoneTowerTop01/CastleStoneTowerTop01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_FarmHouseWindmill"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/FarmHouseWindmill/FarmHouseWindmill.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_GuardHouse"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/GuardHouse/GuardHouse.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HalloFtheDead01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HalloFtheDead01/HalloFtheDead01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_House02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/House02/House02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseBanneredmare"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseBanneredmare/HouseBanneredmare.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseBlackSmith"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseBlackSmith/HouseBlackSmith.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseGreymane"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseGreymane/HouseGreymane.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseMeadery01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseMeadery01/HouseMeadery01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseMeadery01WareHouse"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseMeadery01WareHouse/HouseMeadery01WareHouse.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseShack01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseShack01/HouseShack01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseShack02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseShack02/HouseShack02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStables01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseStables01/HouseStables01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStores01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseStores01/HouseStores01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseStores02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseStores02/HouseStores02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseWind01/HouseWind01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseWind02/HouseWind02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseWind03/HouseWind03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HouseWind04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/HouseWind04/HouseWind04.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Jorvaskr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Jorvaskr01/Jorvaskr01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/MarketStand01/MarketStand01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/MarketStand02/MarketStand02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_MarketStand03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/MarketStand03/MarketStand03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StablePlatend01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StablePlatend01/StablePlatend01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StablePlatstr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StablePlatstr01/StablePlatstr01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableStr01end"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StableStr01end/StableStr01end.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableWallbackStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StableWallbackStr01/StableWallbackStr01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StableWallShortStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/StableWallShortStr01/StableWallShortStr01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Templeofk01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Templeofk01/Templeofk01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Trellisarch01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/Trellisarch01/Trellisarch01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrellisColumn01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/TrellisColumn01/TrellisColumn01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrellisComplete01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/TrellisComplete01/TrellisComplete01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_TrelliSlattice01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_Building/TrelliSlattice01/TrelliSlattice01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region StoneWork

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_StairsWater01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/StairsWater01/StairsWater01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCap01/WallCap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCap01Nohighcol"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCap01Nohighcol/WallCap01Nohighcol.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve30up128"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve30up128/WallCurve30up128.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve30up128a"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve30up128a/WallCurve30up128a.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve45"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve45/WallCurve45.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve45up128"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve45up128/WallCurve45up128.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallCurve60"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallCurve60/WallCurve60.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallGateDrawBridge01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallGateDrawBridge01/WallGateDrawBridge01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallGatetarTrap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallGatetarTrap01/WallGatetarTrap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallMainGate01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallMainGate01/WallMainGate01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallMainGate01ext"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallMainGate01ext/WallMainGate01ext.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallMainGateHouse01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallMainGateHouse01/WallMainGateHouse01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile01/WallRubblePile01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile02/WallRubblePile02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile03/WallRubblePile03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile04/WallRubblePile04.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRubblePile05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRubblePile05/WallRubblePile05.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRuinsFree01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRuinsFree01/WallRuinsFree01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallRuinsFree01Entance01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallRuinsFree01Entance01/WallRuinsFree01Entance01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01/WallStr01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01nohighcoll"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01nohighcoll/WallStr01nohighcoll.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01stockades01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01stockades01/WallStr01stockades01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01stockades02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01stockades02/WallStr01stockades02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStr01stockades03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStr01stockades03/WallStr01stockades03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrrubbled01free"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrrubbled01free/WallStrrubbled01free.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrrubbled01short"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrrubbled01short/WallStrrubbled01short.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrrubbled02free"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrrubbled02free/WallStrrubbled02free.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrup128"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrup128/WallStrup128.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrup128shift128"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrup128shift128/WallStrup128shift128.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallStrup128shift128nohighcol"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallStrup128shift128nohighcol/WallStrup128shift128nohighcol.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTierdivide01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTierdivide01/WallTierdivide01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTotem01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTotem01/WallTotem01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTower01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTower01/WallTower01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTower01collpiece"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTower01collpiece/WallTower01collpiece.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTowercap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTowercap01/WallTowercap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTowerChunk01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTowerChunk01/WallTowerChunk01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTowerFree01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTowerFree01/WallTowerFree01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallTowerRubbled01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallTowerRubbled01/WallTowerRubbled01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_WallWaterGrate01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Skyrim_WhiteRun_StoneWork/WallWaterGrate01/WallWaterGrate01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion


	/* 임시 */
#pragma region Dungeon_SewerBarrel

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCorner01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCorner01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCorner02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCorner02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCorner03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCorner03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCorner04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCorner04.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCornerCap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCornerCap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCornerV01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCornerV01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombDoorc01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombDoorc01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomBendCap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoomBendCap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombMid01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombMid01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombMid02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombMid02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombMid03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombMid03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofL01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofL01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofL02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofL02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofL03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofL03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofR01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofR01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofR02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofR02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofR03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofR03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombStairs01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombWall01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombWall01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombWall02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombWall02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombWall03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombWall03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombWall04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombWall04.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomDropDown01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoomDropDown01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombDoor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombDoor01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofrDoorway"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofrDoorway.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region Dungeon_SewerHall

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DirtMound01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/DirtMound01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DirtMound02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/DirtMound02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Hall1Way02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/Hall1Way02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallArch01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallArch01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallArch02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallArch02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallDeadEnd01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallDeadEnd01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallGutter01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallGutter01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallGutter02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallGutter02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallLadderDoor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallLadderDoor01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallSewerHole01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallSewerHole01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallStairs01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region Dungeon_SewerRoom

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCore01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCore01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCoreCap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCoreCap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCoreWall01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCoreWall01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCoreWall02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCoreWall02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCoreWall03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCoreWall03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomDoor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomDoor01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomPillar02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomPillar02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomPillar03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomPillar03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomWall01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomWall01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomWall02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomWall02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomWall03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomWall03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomWall04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomWall04.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion 


	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CLoader::Set_ProtoType_DungeonMesh(LEVELID _eLevel)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_matrix matInitialize = XMMatrixIdentity();
	matInitialize = XMMatrixScaling(0.01f, 0.01f, 0.01f);

#pragma region Dungeon_SewerBarrel

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCorner01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCorner01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCorner02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCorner02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCorner03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCorner03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCorner04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCorner04.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCornerCap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCornerCap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombCornerV01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombCornerV01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombDoorc01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombDoorc01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomBendCap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoomBendCap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombMid01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombMid01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombMid02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombMid02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombMid03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombMid03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofL01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofL01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofL02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofL02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofL03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofL03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofR01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofR01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofR02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofR02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofR03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofR03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombStairs01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombWall01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombWall01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombWall02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombWall02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombWall03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombWall03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombWall04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombWall04.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomDropDown01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoomDropDown01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombDoor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombDoor01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoombRoofrDoorway"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerBarrel/RoombRoofrDoorway.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region Dungeon_SewerHall

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DirtMound01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/DirtMound01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_DirtMound02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/DirtMound02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_Hall1Way02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/Hall1Way02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallArch01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallArch01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallArch02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallArch02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallDeadEnd01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallDeadEnd01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallGutter01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallGutter01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallGutter02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallGutter02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallLadderDoor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallLadderDoor01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallSewerHole01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallSewerHole01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_HallStairs01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerHall/HallStairs01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion

#pragma region Dungeon_SewerRoom

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCore01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCore01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCoreCap01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCoreCap01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCoreWall01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCoreWall01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCoreWall02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCoreWall02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomCoreWall03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomCoreWall03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomDoor01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomDoor01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomPillar02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomPillar02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomPillar03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomPillar03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomWall01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomWall01.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomWall02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomWall02.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomWall03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomWall03.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoType_Component(_eLevel, TEXT("ProtoType_Component_Model_RoomWall04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resource/BinaryFBX/NonAnim/Dungeon_SewerRoom/RoomWall04.bin", matInitialize, CModel::TYPE_NONANIM))))
		return E_FAIL;

#pragma endregion 

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Set_ProtoType_WhiteObject()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	/* 임시 */
	/* Skeever */
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Skeever"),
		CSkeever::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Falmer_UnEquip */
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Falmer_Unequip"),
		CFalmer_UnEquip::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region SkyrimTerrain
	/* Terrain */ 
	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_ArcadiaBase01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BanneredMareBase01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BlackSmithBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BreezeBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BridgeStone01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CarlottaPlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleStairs01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CloudDistrictTerrainlod"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CommonHousePlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CompanionBase01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_DrunkenHuntsmanBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_DryGoodsBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GreatHousePlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GreatHousePlatform02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GreymaneBase"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GuardHouse01Arch01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HalloFdeadPlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MainRoadMarket"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MainRoadPlains01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MainRoadPlains02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PlainsDistrictTerrainlod"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondStairs01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondStairs02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondWallCor01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondWallStr01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondWallStr02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_PondWallStr03"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_ShacksBase01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_ShacksBase02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_SkyForge01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_SkyForge01Door01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_SkyForge01lod"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StairsPlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StairsStreamExit01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TerCloud01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TerCloud02"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TerCloud03"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TerWind01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TreeCircle01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TreeCircleWater01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_UlfberhPlatform01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WaterPool01"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WindDistrictTerrainLod"),
		CSkyrimTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region Building

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_BreezeHome"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Bridge01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Bridge02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleEntrance01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleMainBuilding01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleStoneTower01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleStoneTower01Porch"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleStoneTower01Porchint"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleStoneTowerTop01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_CastleBridge"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_FarmHouseWindmill"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_GuardHouse"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HalloFtheDead01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_House02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseBanneredmare"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseBlackSmith"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseGreymane"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseMeadery01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseMeadery01WareHouse"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseShack01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseShack02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStables01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStores01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseStores02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind03"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HouseWind04"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Jorvaskr01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand02"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_MarketStand03"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StablePlatend01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StablePlatstr01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableStr01end"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableWallbackStr01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StableWallShortStr01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Templeofk01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Trellisarch01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrellisColumn01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrellisComplete01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_TrelliSlattice01"),
		CBuilding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region StoneWork

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_StairsWater01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCap01Nohighcol"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve30up128"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve30up128a"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve45"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve45up128"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallCurve60"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallGateDrawBridge01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallGatetarTrap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallMainGate01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallMainGate01ext"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallMainGateHouse01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile04"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRubblePile05"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRuinsFree01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallRuinsFree01Entance01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01nohighcoll"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01stockades01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01stockades02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStr01stockades03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrrubbled01free"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrrubbled01short"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrrubbled02free"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrup128"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrup128shift128"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallStrup128shift128nohighcol"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTierdivide01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTotem01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTower01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTower01collpiece"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTowercap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTowerChunk01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTowerFree01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallTowerRubbled01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_WallWaterGrate01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	/* 임시 */

#pragma region Dungeon_SewerBarrel

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCorner01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCorner02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCorner03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCorner04"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCornerCap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCornerV01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombDoorc01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomBendCap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombMid01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombMid02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombMid03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofL01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofL02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofL03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofR01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofR02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofR03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombStairs01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombWall01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombWall02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombWall03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombWall04"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomDropDown01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombDoor01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofrDoorway"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Dungeon_SewerHall

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_DirtMound01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_DirtMound02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Hall1Way02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallArch01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallArch02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallDeadEnd01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallGutter01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallGutter02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallLadderDoor01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallSewerHole01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallStairs01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion 

#pragma region Dungeon_SewerRoom

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCore01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCoreCap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCoreWall01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCoreWall02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCoreWall03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomDoor01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomPillar02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomPillar03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomWall01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomWall02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomWall03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomWall04"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion 


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Set_ProtoType_DungeonObject()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region Dungeon_SewerBarrel

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCorner01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCorner02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCorner03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCorner04"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCornerCap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombCornerV01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombDoorc01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomBendCap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombMid01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombMid02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombMid03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofL01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofL02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofL03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofR01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofR02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofR03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombStairs01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombWall01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombWall02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombWall03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombWall04"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomDropDown01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombDoor01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoombRoofrDoorway"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Dungeon_SewerHall

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_DirtMound01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_DirtMound02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_Hall1Way02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallArch01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallArch02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallDeadEnd01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallGutter01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallGutter02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallLadderDoor01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallSewerHole01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_HallStairs01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion 

#pragma region Dungeon_SewerRoom

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCore01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCoreCap01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCoreWall01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCoreWall02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomCoreWall03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomDoor01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomPillar02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomPillar03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomWall01"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomWall02"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomWall03"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ProtoObject(TEXT("ProtoType_GameObject_RoomWall04"),
		CStoneWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion 

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID eNextLevel)
{
	CLoader* pInstance = new CLoader(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Fail Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
