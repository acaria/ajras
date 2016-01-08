#include "GameCamera.h"
#include "CoreLib.h"
#include "Defines.h"

GameCamera::GameCamera(ViewNode* playground, cc::Rect canvasRect, float initScale):
    playground(playground),
canvasRect(canvasRect), frameRect(cc::Rect::ZERO),
    centerPos(canvasRect.size / 2)
{
    this->curScale = initScale;
    this->playground->setScale(initScale);
    
    auto mListener = cc::EventListenerMouse::create();
    mListener->onMouseDown = [this](cc::Event *event) {
        cc::Vec2 minColSize = {20,20};
        cc::EventMouse* e = static_cast<cc::EventMouse*>(event);
        
        auto cameraPos = centerPos - curPosition;
        cc::Vec2 roomPos = {
            this->canvasRect.origin.x + cameraPos.x,
            this->canvasRect.origin.y + cameraPos.y
        };
        
        cc::Vec2 pos = {e->getCursorX() - roomPos.x - this->canvasRect.origin.x,
                        e->getCursorY() - roomPos.y - this->canvasRect.origin.y};
        
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
                    //here: detect single touch began
                    //...
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
        bool once = true;
        for(auto touch : touches)
        {
            if (once && cameraID.size() < 2)
            {
                once = false;
                
                cc::Point pRelative = touch->getLocation() - this->canvasRect.origin;
                cc::Point pos = (pRelative - this->playground->getPosition()) / curScale;
                
                if ((touch->getStartLocation() - touch->getLocation()).length() < def::touchTreshold)
                {
                    this->onTouch(pos);
                }
                else
                {
                    auto startPos = ((touch->getStartLocation() - this->canvasRect.origin) -
                                     this->playground->getPosition())/ curScale;
                    this->onSwipe(startPos, pos);
                }
                
            }
            cameraID.erase(touch->getID());
        }
    };
    
    this->playground->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
            mListener, this->playground);
    this->playground->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
            tListener, this->playground);
    
}

void GameCamera::setFrameBounds(cc::Rect bounds)
{
    this->frameRect = bounds;
    this->updatePos();
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
    if (focus.enabled)
    {
        this->curPosition = {
            lib::clamp(this->curPosition.x,
                focus.target.x + FOCUS_MARGIN - centerPos.x / 2 / curScale,
                focus.target.x - FOCUS_MARGIN + centerPos.x / 2 / curScale),
            lib::clamp(this->curPosition.y,
                focus.target.y + FOCUS_MARGIN - centerPos.y / 2 / curScale,
                focus.target.y - FOCUS_MARGIN + centerPos.y / 2 / curScale)
        };
    }
    
    if (!frameRect.equals(cc::Rect::ZERO))
    {
        this->curPosition = {
            lib::clamp(this->curPosition.x, frameRect.origin.x + centerPos.x / curScale,
                                            frameRect.getMaxX() - (centerPos.x / curScale)),
            lib::clamp(this->curPosition.y, frameRect.origin.y + centerPos.y / curScale,
                                            frameRect.getMaxY() - (centerPos.y / curScale))
        };
    }

    this->playground->setPosition(centerPos - this->curPosition * curScale);
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
    if (focus.enabled)
    {
        this->curPosition = {
            lib::clamp(pos.x,
                       focus.target.x + FOCUS_MARGIN - centerPos.x / 2 / curScale,
                       focus.target.x - FOCUS_MARGIN + centerPos.x / 2 / curScale),
            lib::clamp(pos.y,
                       focus.target.y + FOCUS_MARGIN - centerPos.y / 2 / curScale,
                       focus.target.y - FOCUS_MARGIN + centerPos.y / 2 / curScale)
        };
    }
    else
    {
        this->curPosition = pos;
    }
    
    if (!frameRect.equals(cc::Rect::ZERO))
    {
        this->curPosition = {
            lib::clamp(this->curPosition.x, frameRect.origin.x + centerPos.x / curScale,
                       frameRect.getMaxX() - (centerPos.x / curScale)),
            lib::clamp(this->curPosition.y, frameRect.origin.y + centerPos.y / curScale,
                       frameRect.getMaxY() - (centerPos.y / curScale))
        };
    }
    
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