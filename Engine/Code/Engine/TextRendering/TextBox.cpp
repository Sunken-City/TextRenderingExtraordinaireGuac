#include "Engine/TextRendering/TextBox.hpp"
#include "Engine/Input/XMLUtils.hpp"


//-----------------------------------------------------------------------------------------------
TextBox::TextBox(const Vector3& bottomLeft, const Vector3& topRight, const Vector3& upVector, const Vector3& rightVector, class BitmapFont* baseFont)
	: m_bottomLeft(bottomLeft)
	, m_topRight(topRight)
	, m_baseFont(baseFont)
{
	
}


//-----------------------------------------------------------------------------------------------
void TextBox::SetFromXMLNode(const struct XMLNode& node)
{
	m_fragments = StringEffectFragment::GetStringFragmentsFromXML(node);

}