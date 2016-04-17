#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/TextRendering/StringEffectFragment.hpp"


//-----------------------------------------------------------------------------------------------
class TextBox
{
public:
	TextBox(const Vector3& bottomLeft, const Vector3& topRight, class BitmapFont* baseFont);
	void SetFromXMLNode(const struct XMLNode& node);

private:
	std::vector<StringEffectFragment> m_fragments;
	Vector3 m_bottomLeft;
	Vector3 m_topRight;
	class BitmapFont* m_baseFont;
};