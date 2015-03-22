#include "Headers.h"

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

Dir& Dir::opposite()
{
    unsigned newValue = None;
    if ((this->value & Left) == Left)
        newValue |= Right;
    if ((this->value & Right) == Right)
        newValue |= Left;
    if ((this->value & Up) == Up)
        newValue |= Down;
    if ((this->value & Down) == Down)
        newValue |= Up;
    this->value = newValue;
    return *this;
}

Dir& Dir::turnLeft()
{
    unsigned newValue = None;
    if ((this->value & Left) == Left)
        newValue |= Down;
    if ((this->value & Right) == Right)
        newValue |= Up;
    if ((this->value & Up) == Up)
        newValue |= Left;
    if ((this->value & Down) == Down)
        newValue |= Right;
    this->value = newValue;
    return *this;
}

Dir& Dir::turnRight()
{
    unsigned newValue = None;
    if ((this->value & Left) == Left)
        newValue |= Up;
    if ((this->value & Right) == Right)
        newValue |= Down;
    if ((this->value & Up) == Up)
        newValue |= Right;
    if ((this->value & Down) == Down)
        newValue |= Left;
    this->value = newValue;
    return *this;
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

void Dir::reset()
{
    this->value = None;
}
