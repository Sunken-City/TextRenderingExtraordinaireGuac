#pragma once

class Entity;
class Agent;
class Item;

struct AttackData
{
    AttackData() {};
    AttackData(Agent* attacker, Entity* target, int damageDie, const char* attackVerb = "Slapped");
    Entity* target;
    Agent* attacker;
    int damageDie; //Support a range like 2-5
    bool didAttackHit;
    float accuracy;
    int damageDealt;
    const char* attackVerb;
    //ResultEnum result;
    //Item* weapon;
    //float chanceToHit;
};

class CombatSystem
{
public:
    CombatSystem() {};
    ~CombatSystem() {};

    static void PrintAttackToMessageLog(const AttackData& data);
    static CombatSystem* instance;
    //Player* m_playerReference;
};