#include "Game/Generators/Generator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XMLUtils.hpp"

GeneratorRegistrationMap* GeneratorRegistration::s_generatorRegistrationMap = nullptr;

//-----------------------------------------------------------------------------------
GeneratorRegistration::GeneratorRegistration(const std::string& name, GeneratorCreationFunction* creationFunction, GenerationProcessCreationFunc* dataCreationFunction) 
	: m_name(name)
	, m_creationFunction(creationFunction)
	, m_dataCreationFunction(dataCreationFunction)
{
	if (!s_generatorRegistrationMap)
	{
		s_generatorRegistrationMap = new GeneratorRegistrationMap();
	}
	//Find, verify that this hasn't been already inserted into our map    
	//s_generatorRegistrationMap->find(name);
	s_generatorRegistrationMap->insert(GeneratorRegistrationMapPair(name, this));
}

//-----------------------------------------------------------------------------------
Generator* GeneratorRegistration::CreateGeneratorByName(const std::string& name)
{
	GeneratorRegistration* generatorRegistration = nullptr;
	ASSERT_OR_DIE(s_generatorRegistrationMap, "s_GenerationRegistrationMap doesn't exist");
	GeneratorRegistrationMap::iterator gRegIter = s_generatorRegistrationMap->find(name);
	if (gRegIter != s_generatorRegistrationMap->end())
	{
		generatorRegistration = gRegIter->second;
		Generator* generator = nullptr;
		generator = (*generatorRegistration->m_creationFunction)(generatorRegistration->m_name);
		return generator;
	}
	else
	{
		ERROR_AND_DIE("Generator didn't exist!");
		return nullptr;
	}
}

//-----------------------------------------------------------------------------------
Generator::Generator()
{

}

//-----------------------------------------------------------------------------------
Generator::~Generator()
{

}

//-----------------------------------------------------------------------------------
void Generator::InitializeMap(Map*)
{

}

//-----------------------------------------------------------------------------------
void Generator::FinalizeMap(Map* map)
{
	for (Cell& cell : map->m_cellGrid)
	{
		if (cell.m_type == Cell::STONE)
		{
			map->SetUnrenderable(cell);
		}
	}
}

//-----------------------------------------------------------------------------------
GenerationProcessData::GenerationProcessData(const XMLNode& node)
{
	m_generatorName = XMLUtils::GetAttribute(node, "generator");
	std::string stepString = XMLUtils::GetAttribute(node, "steps");
	std::vector<std::string>* numbers = SplitString(stepString, "~");
	if (numbers->size() == 1)
	{
		int numberOfSteps = stoi(numbers->at(0));
		m_rangeSteps = Vector2Int(numberOfSteps, numberOfSteps);
	}
	else if (numbers->size() == 2)
	{
		m_rangeSteps = Vector2Int(stoi(numbers->at(0)), stoi(numbers->at(1)));
	}
	else
	{
		ERROR_AND_DIE(Stringf("Range data was invalid for generator %s", m_generatorName.c_str()));
	}
}
