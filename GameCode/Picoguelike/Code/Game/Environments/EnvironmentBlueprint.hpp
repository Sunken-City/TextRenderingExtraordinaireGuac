#pragma once
#include "Game/Generators/Generator.hpp"
#include "ThirdParty/Parsers/XMLParser.hpp"
#include <string>
#include <vector>

class EnvironmentBlueprint
{
public:
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	EnvironmentBlueprint(const XMLNode& blueprintNode, const std::string& filenameWithoutExtensions);

	//STATIC FUNCTIONS//////////////////////////////////////////////////////////////////////////
	static void LoadEnvironmentBlueprints();


	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	static std::map<std::string, EnvironmentBlueprint*> s_envirnomentBlueprints;
	std::string m_name;
	Vector2Int m_size;
	std::vector<GenerationProcessData*> m_generationProcesses;

	//INTERNAL FUNCTIONS//////////////////////////////////////////////////////////////////////////
protected:
	void PopulateFromXMLNode(const XMLNode& blueprintNode);
};