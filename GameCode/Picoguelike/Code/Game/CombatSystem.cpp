#include "Game/CombatSystem.hpp"
#include "Game/MessageLog.hpp"
#include "Game/Entities/Agent.hpp"
#include "Engine/Core/StringUtils.hpp"

CombatSystem* CombatSystem::instance = nullptr;

//-----------------------------------------------------------------------------------
void CombatSystem::PrintAttackToMessageLog(const AttackData& data)
{
    const std::string xmlData = Stringf("<Text value = \"[[%s]] [[%s]] [[%s]] for [[%i]] damage!\" color = \"FFFF00\">      \
                                            <Effect color1 = \"FF0000\" color2 = \"FFFF00\" wave = \"0.5\" />               \
                                            <Effect color1 = \"FFFF00\" color2 = \"FF8000\" shake = \"true\" />             \
                                            <Effect color1 = \"00FF00\" color2 = \"8000FF\" wave = \"0.5\" />               \
                                            <Effect dilate = \".5\" //>                                                     \
                                         </Text>"
                                        , data.attacker->m_name.c_str(), data.attackVerb, data.target->m_name.c_str(), data.damageDealt);
    MessageLog::instance->LogMessage(xmlData, MessageLog::COMBAT);
}

AttackData::AttackData(Agent* att, Entity* targ, int dmgDie, const char* attVerb /*= "Slapped"*/)
    : attacker(att)
    , target(targ)
    , damageDie(dmgDie)
    , attackVerb(attVerb)
{

}
