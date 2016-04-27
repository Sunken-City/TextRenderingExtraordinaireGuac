#include "Game/MessageLog.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Input/XMLUtils.hpp"

MessageLog* MessageLog::instance = nullptr;

//-----------------------------------------------------------------------------------
MessageLog::MessageLog()
    : m_font(BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"))
{
    for (int i = 0; i < NUM_CONSOLE_LINES; ++i)
    {
        m_textBoxes[i] = new TextBox(Vector3(0.0f, (25.0f * i) + 25.0f, 0.0f), Vector3::UP, Vector3::RIGHT, 800.0f, 25.0f, 1.0f, m_font);
        m_textBoxes[i]->ShowBox();
    }
    m_textBoxes[0]->SetFromXMLNode(XMLUtils::OpenXMLDocument("D:/pass2.xml").getChildNode());
    m_consoleHistory.push_back(m_textBoxes[0]->m_textRenderers);
}

//-----------------------------------------------------------------------------------
MessageLog::~MessageLog()
{
    for (std::vector<MeshRenderer*>* stringMeshes : m_consoleHistory)
    {
        for (MeshRenderer* stringMesh : *stringMeshes)
        {
            delete stringMesh;
        }
    }
    for (int i = 0; i < NUM_CONSOLE_LINES; ++i)
    {
        delete m_textBoxes[i];
    }
}

//-----------------------------------------------------------------------------------
void MessageLog::Update(float deltaSeconds)
{
    for (int i = 0; i < NUM_CONSOLE_LINES; ++i)
    {
        m_textBoxes[i]->Update(deltaSeconds);
    }
}

//-----------------------------------------------------------------------------------
void MessageLog::Render()
{
    for (int i = 0; i < NUM_CONSOLE_LINES; ++i)
    {
        m_textBoxes[i]->Render();
    }
}

//-----------------------------------------------------------------------------------
void MessageLog::LogMessage(const std::string xmlMessage, MessageType type /*= MessageType::FLAVOR*/)
{
    //Move up the messages in the log.
    for (int i = NUM_CONSOLE_LINES - 1; i > 0; --i)
    {
        m_textBoxes[i]->m_textRenderers = m_textBoxes[i - 1]->m_textRenderers;
        m_textBoxes[i - 1]->m_textRenderers = nullptr;
    }
    m_textBoxes[0]->SetFromXMLNode(XMLUtils::ParseXMLFromString(xmlMessage));
    m_consoleHistory.push_back(m_textBoxes[0]->m_textRenderers);
}
