#include "Game/Entities/NPCFactory.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputOutputUtils.hpp"
#include "Game/Entities/NPC.hpp"
#include "Game/Behaviors/Behavior.hpp"

std::map<std::string, NPCFactory*> NPCFactory::s_NPCFactories;

//-----------------------------------------------------------------------------------
NPCFactory::NPCFactory(const XMLNode& npcBlueprintNode)
{
    PopulateFromXMLNode(npcBlueprintNode);
}

//-----------------------------------------------------------------------------------
void NPCFactory::LoadAllNPCs()
{
    std::vector<std::string>* files = GetFileNamesInFolder("Data/XML/NPCs/*.xml");
    for (std::string fileName : *files)
    {
        XMLNode root = XMLUtils::OpenXMLDocument("Data/XML/NPCs/" + fileName);
        std::vector<std::string>* words = SplitString(fileName, ".");

        //Get NPCBlueprints
        XMLNode npcBlueprintsNode = root.getChildNode(0);
        std::vector<XMLNode> npcBlueprints = XMLUtils::GetChildren(npcBlueprintsNode);
        for (XMLNode npcBlueprint : npcBlueprints)
        {
            if (!npcBlueprint.IsContentEmpty())
            {
                s_NPCFactories[XMLUtils::GetAttribute(npcBlueprint, "name")] = new NPCFactory(npcBlueprint);
            }
        }
        delete words;
    }
    delete files;
}

//-----------------------------------------------------------------------------------
NPC* NPCFactory::Create()
{
    return new NPC(*m_templateNPC);
}

//-----------------------------------------------------------------------------------
void NPCFactory::PopulateFromXMLNode(const XMLNode& npcBlueprintNode)
{
    m_factoryName = XMLUtils::GetAttribute(npcBlueprintNode, "name");
    m_templateNPC = new NPC();
    std::string maxHealth = XMLUtils::GetAttribute(npcBlueprintNode, "maxHealth");
    m_templateNPC->m_maxHealth = maxHealth.empty() ? 10 : stoi(maxHealth);
    m_templateNPC->m_health = m_templateNPC->m_maxHealth;
    m_templateNPC->m_glyph = XMLUtils::GetAttribute(npcBlueprintNode, "glyph").c_str()[0];
    std::string colorString = XMLUtils::GetAttribute(npcBlueprintNode, "color");
    if (colorString.at(1) != 'x') //Format R,G,B
    {
        std::vector<std::string>* strings = SplitString(colorString, ",");
        m_templateNPC->m_color = RGBA(stoi(strings->at(0)) / 255.0f, stoi(strings->at(1)) / 255.0f, stoi(strings->at(2)) / 255.0f);
    }
    else //Format 0xRRGGBBAA
    {
        unsigned int hexColor = stoul(colorString, nullptr, 0);
        m_templateNPC->m_color = RGBA(hexColor);
    }

    //Factions
    XMLNode factionsNode = XMLUtils::GetChildNodeAtPosition(npcBlueprintNode, "Factions");

    //Behaviors
    XMLNode behaviorsNode = XMLUtils::GetChildNodeAtPosition(npcBlueprintNode, "Behaviors");
    std::vector<XMLNode> behaviors = XMLUtils::GetChildren(behaviorsNode);
    for (XMLNode behaviorNode : behaviors)
    {
        if (!behaviorNode.isEmpty())
        {
            //Add Behavior!
            std::string behaviorName = behaviorNode.getName();
            Behavior* behavior = BehaviorRegistration::CreateBehaviorByNameAndXML(behaviorName, behaviorNode);
            if (behavior)
            {
                m_templateNPC->m_behaviors.push_back(behavior);
            }
        }
    }
}

//-----------------------------------------------------------------------------------
NPC* NPCFactory::GetNPCAt(int index)
{
    auto factoryIterator = NPCFactory::s_NPCFactories.begin();
    std::advance(factoryIterator, index);
    return factoryIterator->second->Create();
}
