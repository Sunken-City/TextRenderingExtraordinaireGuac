#include "Engine/TextRendering/TextBox.hpp"
#include "Engine/Input/XMLUtils.hpp"


//-----------------------------------------------------------------------------------------------
TextBox::TextBox(const Vector3& bottomLeft, const Vector3& upVector, const Vector3& rightVector, float width, float height, class BitmapFont* baseFont)
	: m_bottomLeft(bottomLeft)
	, m_upVector(upVector)
	, m_rightVector(rightVector)
	, m_baseFont(baseFont)
	, m_width(width)
	, m_height(height)
{
}


//-----------------------------------------------------------------------------------------------
void TextBox::SetFromXMLNode(const struct XMLNode& node)
{
	m_fragments = StringEffectFragment::GetStringFragmentsFromXML(node);
	
	for (;;)
	{
		/*if ()*/
	}
}