#include "GameCamera.h"
#include "CoreLib.h"

GameCamera::GameCamera(cc::Node* playground, cc::Rect bounds):
    playground(playground), groundSize(bounds.size),
    canvasRect(bounds), innerAreaRect(bounds),
    centerPos(bounds.size / 2)
{
    this->playground->setScale(1.0f);
    
    auto mListener = cc::EventListenerMouse::create();
    mListener->onMouseDown = [this](cc::Event *event) {
        cc::Vec2 minColSize = {20,20};
        cc::EventMouse* e = static_cast<cc::EventMouse*>(event);
        
        auto cameraPos = centerPos - curPosition;
        cc::Vec2 roomPos = {
            innerAreaRect.origin.x + cameraPos.x,
            innerAreaRect.origin.y + cameraPos.y
        };
        
        cc::Vec2 pos = {e->getCursorX() - roomPos.x - canvasRect.origin.x,
                        e->getCursorY() - roomPos.y - canvasRect.origin.y};
        
        if (e->getMouseButton() == MOUSE_BUTTON_RIGHT)
        {
            this->prevBRMouseLocation = e->getLocation();
        }
    };
    
    mListener->onMouseMove = [this](cc::Event *event) {
        cc::EventMouse* e = static_cast<cc::EventMouse*>(event);
        if (e->getMouseButton() == MOUSE_BUTTON_RIGHT)
        {
            auto translateValue = e->getLocation() - this->prevBRMouseLocation;
         
            this->translate({translateValue.x / 2, -translateValue.y / 2});
            this->prevBRMouseLocation = e->getLocation();
        }
    };
    
    mListener->onMouseScroll = [this](cc::Event *event) {
        cc::EventMouse* e = static_cast<cc::EventMouse*>(event);
        this->addScale(e->getScrollY() / 200.0f);
    };
    
    auto tListener = cc::EventListenerTouchAllAtOnce::create();
    
    tListener->onTouchesBegan = [this](const std::vector<cc::Touch*>& touches, cc::Event* event) {
        for(auto touch : touches)
        {
            cc::Vec2 minColSize = {20,20};
                
            auto touchPos = touch->getLocation();
                
            if (this->canvasRect.containsPoint(touchPos)) // frame zone
            {
                if (this->cameraID.size() < 2)
                {
                    this->cameraID[touch->getID()] = touchPos;
                    if (this->cameraID.size() > 1)
                        {
                            //multitouch on frame zone detected
                            continue;
                        }
                    }
                    auto cameraPos = centerPos - curPosition;
                    cc::Vec2 roomPos = {
                        innerAreaRect.origin.x + cameraPos.x,
                        innerAreaRect.origin.y + cameraPos.y
                    };
                    
                    cc::Vec2 pos = {touchPos.x - roomPos.x - canvasRect.origin.x,
                        touchPos.y - roomPos.y - canvasRect.origin.y};
                }
            }
    };
    
    tListener->onTouchesMoved = [this](const std::vector<cc::Touch*>& touches, cc::Event* event) {
        std::map<int, cc::Point> movedCameraID;
        
        for(auto touch : touches)
        {
            auto touchPos = touch->getLocation();
                
            if (lib::hasKey(cameraID, touch->getID()) && cameraID.size() > 1)
            {
                movedCameraID[touch->getID()] = touchPos;
            }
        }
        
        //compute camera handler
        if (movedCameraID.size() > 0)
        {
            auto firstEl = cameraID.begin();
            auto secondEl = ++cameraID.begin();
            auto oldRect = computeRect(firstEl->second, secondEl->second);
            auto pt1 = lib::hasKey(movedCameraID, firstEl->first) ?
            movedCameraID[firstEl->first]:firstEl->second;
            auto pt2 = lib::hasKey(movedCameraID, secondEl->first) ?
            movedCameraID[secondEl->first]:secondEl->second;
            auto newRect = computeRect(pt1, pt2);
            
            if (newRect.size.width > 10 && newRect.size.height > 10)
            {
                auto translateValue = newRect.origin - oldRect.origin;
                auto scaleValue =  (oldRect.size.width + oldRect.size.height) /
                (newRect.size.width + newRect.size.height);
                this->addScale(1 - scaleValue);
                this->translate(translateValue);
            }
            
            for(auto moved : movedCameraID) //update pos
                cameraID[moved.first] = moved.second;
        }
    };
    
    tListener->onTouchesEnded = [this](const std::vector<cc::Touch*>& touches, cc::Event* event) {
        for(auto touch : touches)
        {
            cameraID.erase(touch->getID());
        }
    };
    
    this->playground->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
            mListener, this->playground);
    this->playground->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
            tListener, this->playground);
    
}

void GameCamera::setInnerArea(cc::Rect bounds)
{
    this->innerAreaRect = bounds;
}

void GameCamera::setTarget(cc::Point pos)
{
    this->curPosition = pos;
    this->updatePos();
}

void GameCamera::translate(cc::Vec2 translation)
{
    if (moving) return;
    this->curPosition -= translation;
    this->updatePos();
}

void GameCamera::setScale(float scale)
{
    if (moving) return;
    this->curScale = lib::clamp(scale, MIN_SCALE, MAX_SCALE);
    this->playground->setScale(this->curScale);
    this->updatePos();
}

void GameCamera::addScale(float value)
{
    if (moving) return;
    curScale = lib::clamp(curScale + value, MIN_SCALE, MAX_SCALE);
    this->playground->setScale(curScale);
    this->updatePos();
}

void GameCamera::updatePos()
{
    if (moving) return;
    auto filteredPos = this->curPosition;
    if (focus.enabled)
    {
        filteredPos = {
            lib::clamp(this->curPosition.x, focus.target.x + FOCUS_MARGIN - centerPos.x / 2,
                                            focus.target.x - FOCUS_MARGIN + centerPos.x / 2),
            lib::clamp(this->curPosition.y, focus.target.y + FOCUS_MARGIN - centerPos.y / 2,
                                            focus.target.y - FOCUS_MARGIN + centerPos.y / 2)
        };
        this->curPosition = filteredPos;
    }

    this->playground->setPosition(centerPos - filteredPos * curScale);
}

void GameCamera::focusTarget(cc::Point pos)
{
    focus.enabled = true;
    focus.target = pos;
    updatePos();
}

void GameCamera::unfocus()
{
    focus.enabled = false;
}

void GameCamera::moveTarget(cocos2d::Vec2 pos, float duration)
{
    this->curPosition.x = pos.x;
    this->curPosition.y = pos.y;

    moving = true;
    auto action = cc::Sequence::create(
        cc::EaseInOut::create(cc::MoveTo::create(duration, centerPos - curPosition * curScale), 5),
        cc::CallFunc::create([this](){
            this->moving = false;
        }),
        NULL);
    action->setTag(CAMERA_ID);
    
    this->playground->runAction(action);
}

cc::Rect GameCamera::computeRect(cc::Point p1, cc::Point p2)
{
    cc::Rect result;
    result.origin.x = (p1.x < p2.x) ? p1.x : p2.x;
    result.size.width = (p1.x < p2.x) ? p2.x - p1.x : p1.x - p2.x;
    result.origin.y = (p1.y < p2.y) ? p1.y : p2.y;
    result.size.height = (p1.y < p2.y) ? p2.y - p1.y : p1.y - p2.y;
    return result;
}