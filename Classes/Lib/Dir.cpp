#include "Dir.h"

Dir::Dir(const Cardinal& e)
{
    this->value = e;
}

Dir::Dir(unsigned d)
{
    this->value = d & (Left | Right | Up | Down);
}

Dir& Dir::operator=(const Dir &rhs)
{
    if (this == &rhs)
        return *this;
    this->value = rhs.value;
    return *this;
}

Dir& Dir::operator=(const unsigned& rhs)
{
    this->value = rhs;
    return *this;
}

cc::Vec2 Dir::toVec()
{
    cocos2d::Vec2 res(
        (contains(Dir::Left) ? -1.0f : 0.0f) + (contains(Dir::Right) ? 1.0f : 0.0f),
        (contains(Dir::Up) ? 1.0f : 0.0f) + (contains(Dir::Down) ? -1.0f : 0.0f));
    return res.getNormalized();
}

bool Dir::operator!=(const Dir&rhs)
{
    return !(*this == rhs);
}

bool Dir::operator==(const Dir &rhs)
{
    return this->value == rhs.value;
}

bool Dir::contains(const Dir &rhs) const
{
    return (this->value & rhs.value) == rhs.value;
}

bool Dir::uncross(const Dir &rhs) const
{
    return (this->value & rhs.value) == 0;
}

bool operator==(const Dir& lhs, const Dir& rhs) {
    return lhs.getRaw() == rhs.getRaw();
}
bool operator!=(const Dir& lhs, const Dir& rhs) {
    return !(lhs == rhs);
}

unsigned Dir::getRaw() const
{
    return value;
}

Dir Dir::rand()
{
    switch(std::rand() % 5)
    {
        case 0: return Dir::Left;
        case 1: return Dir::Right;
        case 2: return Dir::Up;
        case 3: return Dir::Down;
        case 4: return Dir::None;
    }
    return Dir::None;
}

Dir Dir::fromVec(cc::Vec2 r)
{
    cc::Vec2 v = {
        roundf(r.x * 10) / 10,
        roundf(r.y * 10) / 10
    };
    unsigned res = Dir::None;
    
    if (v.x < 0) res |= Dir::Left;
    if (v.x > 0) res |= Dir::Right;
    if (v.y < 0) res |= Dir::Down;
    if (v.y > 0) res |= Dir::Up;
    
    return res;
}

Dir Dir::cardinalFromVec(cc::Vec2 r)
{
    cc::Vec2 v = {
        roundf(r.x * 10) / 10,
        roundf(r.y * 10) / 10
    };
    unsigned res = Dir::None;
    
    if (fabs(v.x) > fabs(v.y))
    {
        if (v.x < 0) res |= Dir::Left;
        if (v.x > 0) res |= Dir::Right;
    }
    
    if (fabs(v.y) > fabs(v.x))
    {
        if (v.y < 0) res |= Dir::Down;
        if (v.y > 0) res |= Dir::Up;
    }
    return res;
}

void Dir::reset()
{
    this->value = None;
}
