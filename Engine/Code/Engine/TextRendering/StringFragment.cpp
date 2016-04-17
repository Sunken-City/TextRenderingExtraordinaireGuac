#include "Engine/TextRendering/StringFragment.hpp"
#include "Engine/Input/XMLUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
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
	std::vector<std::string>* fragmentValues = SplitString(inString, 2, "[[", "]]");
	int effectNodeCount = node.nChildNode();
	int effectNodeCountExpected = fragmentValues->size() / 2;

	GUARANTEE_OR_DIE((fragmentValues->size() & 1) == 0, 
		Stringf("Input string:\n%s\ncannot be parsed.  Make sure effect text is not nested.  Close all effect text blurbs", inString.c_str()));
	GUARANTEE_OR_DIE(effectNodeCount == effectNodeCountExpected, 
		Stringf("Affected fragment and effect node count mismatch.  The input string:\n%s\nexpects %i effect nodes", inString.c_str(), effectNodeCountExpected));

	std::vector<StringFragment> result;
	int childNodeSearchPointer = 0;
	TextEffect defaultTextEffects = GetTextEffect(node);
	for (size_t i = 0; i < fragmentValues->size(); i++)
	{

	}
}


//-----------------------------------------------------------------------------------------------
TextEffect StringFragment::GetTextEffect(const struct XMLNode& node)
{
	TextEffect result;
	std::string wave = XMLUtils::GetAttribute(node, "wave");
	if (wave != "")
	{
		try
		{
			result.wave = std::stof(wave);
		}
		catch (const std::exception&)
		{
			ERROR_AND_DIE(Stringf("wave effect value %s is invalid.  Expected a float", wave.c_str()));
		}
	}

	std::string shake = XMLUtils::GetAttribute(node, "shake");
	if (shake != "")
	{
		if (shake == "true")
		{
			result.shake = true;
		}
		else if (shake == "false");
		else
		{
			ERROR_AND_DIE(Stringf("shake effect value %s is invalid.  Expected \"true\" or \"false\"", shake.c_str()));
		}
	}

	std::string dilate = XMLUtils::GetAttribute(node, "dilate");
	if (dilate != "")
	{
		try
		{
			result.dilate = std::stof(dilate);
		}
		catch (const std::exception&)
		{
			ERROR_AND_DIE(Stringf("dilate effect value %s is invalid.  Expected a float", wave.c_str()));
		}
	}

	std::string pop = XMLUtils::GetAttribute(node, "shake");
	if (pop != "")
	{
		if (pop == "true")
		{
			result.pop = true;
		}
		else if (pop == "false");
		else
		{
			ERROR_AND_DIE(Stringf("pop effect value %s is invalid.  Expected \"true\" or \"false\"", shake.c_str()));
		}
	}

	std::string color = XMLUtils::GetAttribute(node, "color");
	if (color != "")
	{

	}
}