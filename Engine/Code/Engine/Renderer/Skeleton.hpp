#pragma once

#include <vector>
#include <string>
#include "Engine/Math/Matrix4x4.hpp"

class Skeleton
{
public:
	//FUNCTIONS//////////////////////////////////////////////////////////////////////////
	inline int GetLastAddedJointIndex() const { return m_names.size() - 1; };
	void AddJoint(const char* str, int parentJointIndex, Matrix4x4 initialBoneToModelMatrix);

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	std::vector<std::string> m_names;
	std::vector<int> m_parentIndices;
	std::vector<Matrix4x4> m_modelToBoneSpace;
	std::vector<Matrix4x4> m_boneToModelSpace;
};