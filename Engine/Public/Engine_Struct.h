#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		HWND		hWnd;
		WINMODE		eWinMode;
		unsigned int	iWinSizeX, iWinSizeY;

	}GRAPHIC_DESC;

	typedef struct tagLightDesc
	{
		enum TYPE { LIGHT_DIRECTIONAL, LIGHT_POINT, LIGHT_END };

		TYPE eLightType = LIGHT_END;

		// �� ����
		_float4 vLightPos;
		_float  fLightRange;
		// ���⼺ ����
		_float4 vLightDir;

		// ����
		_float4 vDiffuse;
		_float4 vAmbient;
		_float4 vSpecular;

	}LIGHT_DESC;

	typedef struct tagMeshMaterial
	{
		class CTexture* pTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	typedef struct ENGINE_DLL tagVertex_Position_Color
	{
		XMFLOAT3	vPosition;
		XMFLOAT4	vColor;
	}VTXPOSCOL;

	typedef struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];

	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];

	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_NonAnim_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int				iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}VTX_NONANIMMESH;

	typedef struct ENGINE_DLL tagVertex_Anim_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		// �ڽ��� ���¸� �޾ƾ� �� ������ �ε����� ��� ����.
		// �ִ� 4������ ���� �����ϴ�.
		XMUINT4			vBlendIndices;
		// ���� ���¸� �� �ۼ�Ʈ ������ ������ �����ϴ� ����.
		XMFLOAT4		vBlendWeights;

		static const unsigned int				iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}VTX_ANIMMESH;

	typedef struct tagKeyFrame
	{
		float		fTime;
		XMFLOAT3    vScale;
		XMFLOAT4    vRotation;
		XMFLOAT4    vTranslation;

	}KEYFRAME;
}

#endif // Engine_Struct_h__
