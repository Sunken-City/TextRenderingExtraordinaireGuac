#pragma once
#include "Engine/Input/XMLUtils.hpp"
#include <map>
#include <vector>

class Behavior;
class BehaviorRegistration;
class NPC;
class Entity;

//TYPEDEFS//////////////////////////////////////////////////////////////////////////
typedef std::pair<std::string, BehaviorRegistration*> BehaviorRegistrationMapPair;
typedef std::map<std::string, BehaviorRegistration*> BehaviorRegistrationMap;
typedef Behavior* (BehaviorCreationFunction)(const std::string& name, const XMLNode& dataNode);

//-----------------------------------------------------------------------------------
class BehaviorRegistration
{
public:
    //CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
    BehaviorRegistration(const std::string& name, BehaviorCreationFunction* creationFunction);

    //STATIC FUNCTIONS//////////////////////////////////////////////////////////////////////////
    static Behavior* CreateBehaviorByNameAndXML(const std::string& name, const XMLNode& dataNode);

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    static BehaviorRegistrationMap* s_behaviorRegistrationMap;

    std::string m_name;
    BehaviorCreationFunction* m_creationFunction;
};

//-----------------------------------------------------------------------------------
class Behavior
{
public:
    //ENUMS//////////////////////////////////////////////////////////////////////////
    enum Utility
    {
        NONE = 0,
        LOW = 1,
        MEDIUM = 5,
        HIGH = 10,
        NUM_UTILITIES
    };

    //CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
    Behavior();
    Behavior(NPC* owningNPC);
    virtual ~Behavior();
    virtual Behavior* Clone() = 0;

    //FUNCTIONS//////////////////////////////////////////////////////////////////////////
    virtual bool DoesPassChanceToRun() = 0; //Does this behavior run on this frame?
    virtual float CalculateUtility() = 0;
    virtual float Run() = 0;

    //GETTERS//////////////////////////////////////////////////////////////////////////

    //SETTERS//////////////////////////////////////////////////////////////////////////
    virtual void SetAgent(NPC* owningNPC) { m_owningNPC = owningNPC; };
    virtual void SetVisibleEntities(std::vector<Entity*>* visibleEntities) { m_visibleEntities = visibleEntities; };

    //QUERIES//////////////////////////////////////////////////////////////////////////

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    NPC* m_owningNPC;
    std::vector<Entity*>* m_visibleEntities;
};