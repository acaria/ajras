#pragma once

class ViewNode : public cc::Node
{
public:
    virtual ~ViewNode() {}

    virtual void setPosition(const cc::Vec2& pos) override;
    virtual void setPosition(float x, float y) override;
    virtual const cc::Vec2& getPosition() const override;
    virtual cc::Vec3 getPosition3D() const override;
    
private:

    cc::Point actualPosition;
};