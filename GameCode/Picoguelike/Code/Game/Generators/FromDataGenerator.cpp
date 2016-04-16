#include "Game/Generators/FromDataGenerator.hpp"
#include "Engine/Input/XMLUtils.hpp"
#include <string>
#include <vector>

GeneratorRegistration FromDataGenerator::s_fromDataGeneratorRegistration("FromData", &FromDataGenerator::CreateGenerator, &FromDataGenerator::CreateGenerationProcessData);

//-----------------------------------------------------------------------------------
FromDataGenerator::FromDataGenerator(const std::string& name)
{
    m_name = name;
}

FromDataGenerator::~FromDataGenerator()
{

}

void FromDataGenerator::InitializeMap(Map*)
{

}

bool FromDataGenerator::GenerateStep(int&, GenerationProcessData* data, Map* map /*= nullptr*/)
{
    std::string fileName = static_cast<FromDataGenerationProcessData*>(data)->m_filePath;
    XMLNode root = XMLUtils::OpenXMLDocument(fileName);
    XMLNode mapRoot = XMLUtils::GetChildNodeAtPosition(root, "Map");
    std::vector<XMLNode> children = XMLUtils::GetChildren(mapRoot);
    map->InitializeLegend(children[0]);
    map->LoadMap(children[1]);
    return true;
}

FromDataGenerationProcessData::FromDataGenerationProcessData(const XMLNode& node)
    : GenerationProcessData(node)
{
    m_filePath = XMLUtils::GetAttribute(node, "filePath");    
}
