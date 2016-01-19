#pragma once

class CocosHelper
{
public:
    static void addTexture(const std::string& fileName, bool antialias);
    static void addSpriteSheet(const std::string& fileName, bool antialias);
    
    static cc::ActionInterval* blinkActionCreate(const cc::Color3B& color,
                                                 unsigned count,
                                                 float duration);
    
    static void setRecursiveCascadeOpacity(cc::Node* node, bool value);
};