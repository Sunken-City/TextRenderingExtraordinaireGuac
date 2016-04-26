#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/TextRendering/StringEffectFragment.hpp"

#include <deque>


//-----------------------------------------------------------------------------------------------
class TextBox
{
public:
    TextBox(const Vector3& bottomLeft, const Vector3& upVector, const Vector3& rightVector, float width, float height, float scale, class BitmapFont* baseFont);
    ~TextBox();
    void SetFromXMLNode(const struct XMLNode& node);
    void ResetAnimation();
    void Update(float deltaSeconds);
    void Render() const;
    inline void ShowBox() { m_renderBox = true; };
    inline void HideBox() { m_renderBox = false; };

private:
    void EvaluateLine(std::deque<StringEffectFragment>& currLine, std::deque<StringEffectFragment>& fragmentQueue);
    void ConstructMeshes();

    std::vector<class MeshRenderer*> m_textRenderers;
    MeshRenderer* m_borderRenderer;
    //MeshRenderer* m_backgroundRenderer;
    std::vector<StringEffectFragment> m_fragments;
    Vector3 m_bottomLeft;
    Vector3 m_upVector;
    Vector3 m_rightVector;
    class BitmapFont* m_baseFont;
    float m_width;
    float m_height;
    float m_scale;
    float m_totalTimeSinceReset;
    bool m_renderBox;
};