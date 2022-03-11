#pragma once
#include <assimp/scene.h>
#include <vector>
#include <DirectXMath.h>
#include "VertexBoneData.h"
#include "Vertex.h"
#include <map>
#include "AppTimer.h"

struct BoneMatrix
{
	DirectX::XMMATRIX BoneOffset;
	DirectX::XMMATRIX FinalTransformation;
};

class Animation
{
public:
	void Update();
	bool SetAnimIndex(unsigned int index, bool updateBoth = true, float blendDuration = 0.5f, bool temporary = false, float time = 0.f);

protected:
	bool LoadBones(aiMesh* mesh, std::vector<VertexBoneData>& bones, std::vector<Vertex>& vertices);
	void BoneTransform(std::vector<DirectX::XMMATRIX>& Transforms);
	void ReadNodeHierarchy(const aiScene* scene, float& AnimationTime, const aiNode* pNode, const DirectX::XMMATRIX& ParentTransform, int& StopAnimLevel);
	void ReadNodeHierarchy(const aiScene* scene1, const aiScene* scene2, float& AnimationTime1, float& AnimationTime2, const aiNode* pNode1, const aiNode* pNode2, const DirectX::XMMATRIX& ParentTransform, int& StopAnimLevel);
	aiQuaternion CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	aiVector3D CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	aiVector3D CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* p_animation, const std::string pNodeName);
	aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend);

public:
	DirectX::XMMATRIX m_GlobalInverseTransform;
	static const unsigned int MAX_BONES = 100;
	unsigned int m_bone_location[MAX_BONES];
	unsigned int m_NumBones = 0;
	std::vector<BoneMatrix> m_BoneInfo;
	float ticks_per_second = 0.0f;
	std::map<std::string, unsigned int> BoneMapping;
	std::map<std::string, const aiNodeAnim*> node_anims;
	//const aiScene* pScene;
	float m_lastTime;
	float _animationTime0;
	float _animationTime1;
	float _blendingTime;
	float _blendingTimeMul;
	unsigned int _prevAnimIndex;
	bool _updateBoth;
	bool _temporary;
	float _playTime;
	unsigned int _curScene;
	float mAnimationTime = 0.0f;
	float currentAnimTime = 0.0f;
	float animDuration = 0.0f;
	bool stepAnimation = false;
	int animLevel = 0;

	int selected_bone_index;

	AppTimer timer;

	int currAnim = 0;
	int modelAnimID = 1;

	float animSpeed = 30.0f;

	bool bonesLoaded = false;


	std::vector<const aiScene*> scenes;
	unsigned int m_BoneIndex;

	bool bAnimLoaded = false;
};

