#include "Engine/TextRendering/TextBox.hpp"
#include "Engine/Input/XMLUtils.hpp"


//-----------------------------------------------------------------------------------------------
TextBox::TextBox(const Vector3& bottomLeft, const Vector3& topRight, class BitmapFont* baseFont)
	: m_bottomLeft(bottomLeft)
	, m_topRight(topRight)
	, m_baseFont(baseFont)
{

}


//-----------------------------------------------------------------------------------------------
void TextBox::SetFromXMLNode(const struct XMLNode& node)
{

}