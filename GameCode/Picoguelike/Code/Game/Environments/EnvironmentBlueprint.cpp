#include "Game/Environments/EnvironmentBlueprint.hpp"
#include "Engine/Input/InputOutputUtils.hpp"
#include "Engine/Input/XMLUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <vector>
#include <string>

std::map<std::string, EnvironmentBlueprint*> EnvironmentBlueprint::s_envirnomentBlueprints;

//-----------------------------------------------------------------------------------
EnvironmentBlueprint::EnvironmentBlueprint(const XMLNode& blueprintNode, const std::string& filenameWithoutExtensions)
	: m_name(filenameWithoutExtensions)
{
	PopulateFromXMLNode(blueprintNode);
}

//-----------------------------------------------------------------------------------
void EnvironmentBlueprint::LoadEnvironmentBlueprints()
{
	//Already been loaded, clear out the list.
	if (s_envirnomentBlueprints.size() != 0)
	{
		for (std::pair<std::string, EnvironmentBlueprint*> blueprintPair : s_envirnomentBlueprints)
		{
			delete blueprintPair.second;
		}
		s_envirnomentBlueprints.clear();
	}
	std::vector<std::string>* files = GetFileNamesInFolder("Data/XML/Environments/*.xml");
	for (std::string fileName : *files)
	{
		XMLNode root = XMLUtils::OpenXMLDocument("Data/XML/Environments/" + fileName);
		std::vector<std::string>* words = SplitString(fileName, ".");
		s_envirnomentBlueprints[words->at(0)] = new EnvironmentBlueprint(root.getChildNode(0), words->at(0));
		delete words;
	}
	delete files;
}

//-----------------------------------------------------------------------------------
void EnvironmentBlueprint::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
	//Grab EnvironmentBlueprint root properties.
	std::string nameString = XMLUtils::GetAttribute(blueprintNode, "name");
	m_name = nameString == "" ? m_name : nameString;
	std::string sizeString = XMLUtils::GetAttribute(blueprintNode, "size");
	std::vector<std::string>* sizes = SplitString(sizeString, ",");
	m_size = Vector2Int(stoi(sizes->at(0)), stoi(sizes->at(1)));
	std::vector<XMLNode> children = XMLUtils::GetChildren(blueprintNode);
	for (XMLNode child : children)
	{
		if (!child.IsContentEmpty())
		{
			GeneratorRegistrationMap::iterator iter = GeneratorRegistration::s_generatorRegistrationMap->find(XMLUtils::GetAttribute(child, "generator"));
			if (iter != GeneratorRegistration::s_generatorRegistrationMap->end())
			{
				m_generationProcesses.push_back(iter->second->m_dataCreationFunction(child));
			}
		}
	}
	delete sizes;
}

