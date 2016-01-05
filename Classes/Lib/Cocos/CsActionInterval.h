#pragma once

class CC_DLL Flicker : public cc::ActionInterval
{
public:
    static Flicker* create(float duration, float interval,
                           const cc::Vec2& opacity,
                           const cc::Vec2& scaleX, const cc::Vec2& scaleY,
                           const cc::Color3B &minColor, const cc::Color3B &maxColor);
    
    //
    // Overrides
    //
    virtual Flicker* clone() const override;
    virtual Flicker* reverse(void) const  override;
    virtual void startWithTarget(cc::Node *target) override;
    /**
     * @param time in seconds
     */
    virtual void update(float time) override;
    
CC_CONSTRUCTOR_ACCESS:
    Flicker() {}
    virtual ~Flicker() {}
    
    /** initializes the action */
    bool initWithDuration(float duration, float interval,
                          const cc::Vec2& opacity,
                          const cc::Vec2& scaleX,
                          const cc::Vec2& scaleY,
                          const cc::Color3B &minColor,
                          const cc::Color3B &maxColor);
    
protected:
    cc::Vec2 opacity;
    cc::Vec2 scaleX, scaleY;
    cc::Color3B minColor, maxColor;
    double accumulator, lastUpdate;
    float interval;
    
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(Flicker);
    
    static const int kMax = 1024;
    static int r[1024];
    int randIndex;
};