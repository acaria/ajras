#include "ViewNode.h"

void ViewNode::setPosition(const cc::Vec2 &pos)
{
    ViewNode::setPosition(pos.x, pos.y);
}

void ViewNode::setPosition(float x, float y)
{
    this->actualPosition.x = x;
    this->actualPosition.y = y;
    
    float scale = cc::Director::getInstance()->getContentScaleFactor();
    if (scale > 0.0)
    {
        x = round(x * scale) / scale;
        y = round(y * scale) / scale;
    }
    
    Node::setPosition(x, y);
}

const cc::Vec2& ViewNode::getPosition() const
{
    return this->actualPosition;
}

cc::Vec3 ViewNode::getPosition3D() const
{
    return cc::Vec3(this->actualPosition.x, this->actualPosition.y, _positionZ);
}