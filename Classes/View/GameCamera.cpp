#include "GameCamera.h"
#include "CoreLib.h"

GameCamera::GameCamera(cc::Layer* playground, cc::Rect bounds):
    playground(playground),
    groundSize(bounds.size),
    centerPos(bounds.size / 2)
{
    this->playground->setScale(1.0f);
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

float GameCamera::getScale()
{
    return curScale;
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

cc::Point GameCamera::getOrigin() const
{
    return centerPos - curPosition;
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