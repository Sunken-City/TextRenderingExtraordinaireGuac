#pragma once
#include "Engine/TextRendering/TextBox.hpp"

class MeshRenderer;

class MessageLog
{
public:
    //ENUMS//////////////////////////////////////////////////////////////////////////
    enum MessageType
    {
        NPC,
        COMBAT,
        IMPORTANT,
        FLAVOR,
        NUM_TYPES
    };

    MessageLog();
    ~MessageLog();
    void Update(float deltaSeconds);
    void Render();
    void LogMessage(const std::string xmlMessage, MessageType type = MessageType::FLAVOR);

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    static MessageLog* instance;
    static const int NUM_CONSOLE_LINES = 5;
    BitmapFont* m_font;
    TextBox* m_textBoxes[NUM_CONSOLE_LINES];
    std::vector<std::vector<MeshRenderer*>*> m_consoleHistory;
};