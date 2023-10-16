#pragma once

#include "Base.h"

BEGIN(Engine)

class CBin_AIScene final : public CBase
{
public:
#pragma region Node

	// ���
	typedef struct tagNodeDesc
	{
		int							mParentIndex;
		unsigned int				mNumChildren;
		aiString					mName;
		aiMatrix4x4					mTransformation;
	}DESC_NODE;

#pragma endregion

#pragma region Mesh

	// �޽� �� ����Ʈ
	typedef struct tagMeshBoneWeightDesc
	{
		unsigned int mVertexId;
		_float		 mWeight;
	}DESC_MESHBONEWEIGHT;

	// �޽� ��
	typedef struct tagMeshBoneDesc
	{
		aiMatrix4x4			 mOffsetMatrix;
		aiString			 mName;
		unsigned int		 mNumWeights;

		vector<DESC_MESHBONEWEIGHT> mWeights;
	}DESC_MESHBONE;

	// �޽� ��
	typedef struct tagMeshFaceDesc
	{
		unsigned int mIndices[3];
	}DESC_MESHFACE;

	// �޽�
	typedef struct tagMeshDesc
	{
		aiString		mName;
		unsigned int	mMaterialIndex;
		unsigned int	mNumVertices;
		unsigned int	mNumFaces;
		unsigned int	mNumBones;

		vector<DESC_MESHFACE>   mFaces;
		vector<aiVector3D>		mVertices;
		vector<aiVector3D>		mNormals;
		vector<_float2>		mTextureCoords;
		vector<aiVector3D>		mTangents;
		vector<DESC_MESHBONE>   mBones;
	}DESC_MESH;

#pragma endregion

#pragma region Material
	typedef struct tagMaterialDesc
	{
		_int   mBIsReturn[AI_TEXTURE_TYPE_MAX];
		string mStrTextureFilePath[AI_TEXTURE_TYPE_MAX];
	}DESC_MATERIAL;

#pragma region Animation

	// ä�� Ű(time, value)
	typedef struct tagChannelScaleKeyDesc
	{
		double		mTime;
		aiVector3D  mValue;
	}DESC_CHANNELSCALEKEY;

	typedef struct tagChannelRotKeyDesc
	{
		double		 mTime;
		aiQuaternion mValue;
	}DESC_CHANNELROTKEY;

	typedef struct tagChannelPosKeyDesc
	{
		double		mTime;
		aiVector3D  mValue;
	}DESC_CHANNELPOSKEY;

	// ä��
	typedef struct tagAnimationChannelDesc
	{
		aiString		mNodeName;
		unsigned int	mNumScalingKeys;
		unsigned int	mNumRotationKeys;
		unsigned int	mNumPositionKeys;

		vector<DESC_CHANNELSCALEKEY>	mScalingKeys;
		vector<DESC_CHANNELROTKEY>		mRotationKeys;
		vector<DESC_CHANNELPOSKEY>		mPositionKeys;

	}DESC_ANIMATIONCHANNEL;

	// �ִϸ��̼�
	typedef struct tagAnimationDesc
	{
		aiString				mName;
		double					mDuration;
		double					mTicksPerSecond;
		unsigned int			mNumChannels;

		vector<DESC_ANIMATIONCHANNEL>  mChannels;

	}DESC_ANIMATION;

#pragma endregion

private:
	CBin_AIScene();
	virtual ~CBin_AIScene() = default;

public:
	vector<DESC_NODE>		mVecNode;

	unsigned int			mNumMeshes;
	vector<DESC_MESH>		mMeshs;

	unsigned int			mNumMaterials;
	vector<DESC_MATERIAL>	mMaterials;

	unsigned int			m_iNumAnimation;
	vector<DESC_ANIMATION>	mAnimations;

public:
	static CBin_AIScene* Create();
	virtual void Free() override;
};

END

