#pragma once

#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class StringFragment
{
public:
	StringFragment(const std::string& value, const struct XMLNode& effectNode);
	std::vector<StringFragment> GetStringFragmentsFromXML(const struct XMLNode& node);

public:
	std::string m_value;
	std::vector<class TextEffect> m_effects;
};