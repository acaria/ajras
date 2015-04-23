#pragma once

class HealthBar : public cc::Sprite
{
public:
    static HealthBar* create(const std::string& bg,
                             const std::string& full,
                             const std::string& empty);
    
    
    const cc::Size& getContentSize();
    
    void initProperties(unsigned maxHealth, unsigned current);
    void updateProperties(unsigned health);
    
private:
    bool initWithKey(const std::string& bg,
                     const std::string& full,
                     const std::string& empty);
    
    void setLength(float length);
    
    cc::Sprite* bLeft;
    cc::Sprite* bMiddle;
    cc::Sprite* bRight;
    
    double minimumWidth = 0;
    cc::Size contentSize;
    
    std::string fullKey;
    std::string emptyKey;
    
    std::vector<std::pair<cc::Sprite*, cc::Sprite*>> hearts;
};

