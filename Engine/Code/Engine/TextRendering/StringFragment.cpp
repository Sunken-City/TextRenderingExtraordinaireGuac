#include "Engine/TextRendering/StringFragment.hpp"
#include "Engine/TextRendering/TextEffect.hpp"
#include "Engine/Input/XMLUtils.hpp"
#include "ThirdParty/Parsers/XMLParser.hpp"


//-----------------------------------------------------------------------------------------------
StringFragment::StringFragment(const std::string& value, const struct XMLNode& effectNode)
	: m_value(value)
{
	
}


//-----------------------------------------------------------------------------------------------
std::vector<StringFragment> StringFragment::GetStringFragmentsFromXML(const struct XMLNode& node)
{
	std::string inString = XMLUtils::GetAttribute(node, "value");
	return std::vector<StringFragment>();
}