#include "Engine/Renderer/Skeleton.hpp"

void Skeleton::AddJoint(const char* str, int parentJointIndex, Matrix4x4 initialBoneToModelMatrix)
{
	m_names.push_back(std::string(str));
	m_parentIndices.push_back(parentJointIndex);
	m_boneToModelSpace.push_back(initialBoneToModelMatrix);
	Matrix4x4 modelToBoneMatrix = initialBoneToModelMatrix;
	Matrix4x4::MatrixInvert(&modelToBoneMatrix);
	m_modelToBoneSpace.push_back(modelToBoneMatrix);
}
