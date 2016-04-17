#pragma once

#include <string>
#include <vector>
#include "Engine/TextRendering/TextEffect.hpp"


//-----------------------------------------------------------------------------------------------
class StringFragment
{
public:
	StringFragment(const std::string& value);
	static std::vector<StringFragment> GetStringFragmentsFromXML(const struct XMLNode& node);
	static TextEffect GetTextEffect(const struct XMLNode& node);

public:
	std::string m_value;
	TextEffect m_effect;;
};