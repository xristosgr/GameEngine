#pragma once
#include<DirectXMath.h>
#include <vcruntime_string.h>


struct VertexBoneData
{


	VertexBoneData()
	{
		memset(jointIDs, 0, sizeof(jointIDs));
		memset(weights, 0, sizeof(weights));
	}


	void addBoneData(unsigned int& bone_id, float& weight)
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			if (weights[i] == 0.0f)
			{
				jointIDs[i] = bone_id;
				weights[i] = weight;
				return;
			}
		}

	}


	float weights[4] = { 0.0f,0.0f,0.0f,0.0f };
	unsigned int jointIDs[4] = { 0,0,0,0 };
};


