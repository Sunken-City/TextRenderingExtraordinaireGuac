#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/TextRendering/StringEffectFragment.hpp"


//-----------------------------------------------------------------------------------------------
class TextBox
{
public:
	TextBox(const Vector3& bottomLeft, const Vector3& topRight, const Vector3& upVector, const Vector3& rightVector, class BitmapFont* baseFont);
	void SetFromXMLNode(const struct XMLNode& node);

private:
	std::vector<StringEffectFragment> m_fragments;
	Vector3 m_bottomLeft;
	Vector3 m_topRight;
	Vector3 m_topLeft;
	Vector3 m_bottomRight;
	class BitmapFont* m_baseFont;
	float m_width;
	float m_height;
};