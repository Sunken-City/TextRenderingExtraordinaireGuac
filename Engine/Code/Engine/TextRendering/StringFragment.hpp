#pragma once

#include <string>
#include <vector>
#include "Engine/TextRendering/TextEffect.hpp"


//-----------------------------------------------------------------------------------------------
class StringFragment
{
public:
	StringFragment(const std::string& value, const struct XMLNode& effectNode);
	std::vector<StringFragment> GetStringFragmentsFromXML(const struct XMLNode& node);
	TextEffect GetTextEffect(const struct XMLNode& node);

public:
	std::string m_value;
	TextEffect m_effect;;
};