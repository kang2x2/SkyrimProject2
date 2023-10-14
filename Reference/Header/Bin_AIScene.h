#pragma once

#include "Base.h"

BEGIN(Engine)

class CBin_AIScene final : public CBase
{
public:
#pragma region Node

	// 노드
	typedef struct tagNodeDesc
	{
		unsigned int				mParentIndex;
		unsigned int				mNumChildren;
		aiString					mName;
		aiMatrix4x4					mTransformation;
	}DESC_NODE;

#pragma endregion

#pragma region Mesh

	// 메시 본 웨이트
	typedef struct tagMeshBoneWeightDesc
	{
		unsigned int mVertexId;
		_float		 mWeight;
	}DESC_MESHBONEWEIGHT;

	// 메시 본
	typedef struct tagMeshBoneDesc
	{
		aiMatrix4x4			 mOffsetMatrix;
		aiString			 mName;
		unsigned int		 mNumWeights;
		DESC_MESHBONEWEIGHT* mWeights;
	}DESC_MESHBONE;

	// 메시 면
	typedef struct tagMeshFaceDesc
	{
		unsigned int mIndices[3];
	}DESC_MESHFACE;

	// 메시
	typedef struct tagMeshDesc
	{
		unsigned int	mMaterialIndex;
		unsigned int	mNumVertices;
		unsigned int	mNumFaces;
		unsigned int	mNumBones;
		aiString		mName;
		DESC_MESHFACE*	mFaces;
		aiVector3D*		mVertices;
		aiVector3D*		mNormals;
		aiVector3D*		mTextureCoords[AI_MAX_NUMBER_OF_TEXTURECOORDS];
		aiVector3D*		mTangents;
		DESC_MESHBONE*	mBones;
	}DESC_MESH;

#pragma endregion

#pragma region Material
	typedef struct tagMaterialDesc
	{
		_int   mBIsReturn[AI_TEXTURE_TYPE_MAX];
		const char* mStrTextureFilePath[AI_TEXTURE_TYPE_MAX];
	}DESC_MATERIAL;

#pragma region Animation

	// 채널 키(time, value)
	typedef struct tagChannelScaleKeyDesc
	{
		_float		mTime;
		aiVector3D  mValue;
	}DESC_CHANNELSCALEKEY;

	typedef struct tagChannelRotKeyDesc
	{
		_float		 mTime;
		aiQuaternion mValue;
	}DESC_CHANNELROTKEY;

	typedef struct tagChannelPosKeyDesc
	{
		_float		mTime;
		aiVector3D  mValue;
	}DESC_CHANNELPOSKEY;

	// 채널
	typedef struct tagAnimationChannelDesc
	{
		aiString		mNodeName;
		unsigned int	mNumScalingKeys;
		unsigned int	mNumRotationKeys;
		unsigned int	mNumPositionKeys;

		DESC_CHANNELSCALEKEY*	mScalingKeys;
		DESC_CHANNELROTKEY*		mRotationKeys;
		DESC_CHANNELPOSKEY*		mPositionKeys;


	}DESC_ANIMATIONCHANNEL;

	// 애니메이션
	typedef struct tagAnimationDesc
	{
		aiString				mName;
		_float					mDuration;
		_float					mTicksPerSecond;
		unsigned int			mNumChannels;
		DESC_ANIMATIONCHANNEL*  mChannels;

	}DESC_ANIMATION;

#pragma endregion

private:
	CBin_AIScene();
	virtual ~CBin_AIScene() = default;

public:
	vector<DESC_NODE> mVecNode;

	unsigned int mNumMeshes;
	DESC_MESH*		mMeshs;

	unsigned int	mNumMaterials;
	vector<DESC_MATERIAL> mMaterials;

	unsigned int	m_iNumAnimation;
	DESC_ANIMATION*	mAnimations;
public:
	static CBin_AIScene* Create();
	virtual void Free() override;
};

END

