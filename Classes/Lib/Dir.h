#pragma once
#include "Headers.h"

class Dir
{
public:
    enum Cardinal
    {
        None = 0,
        Left = 1,
        Right = 2,
        Up = 4,
        Down = 8
    };
    
    Dir():value(0) {}
    Dir(const Cardinal& e);
    Dir(unsigned d);
    
    Dir& operator=(const Dir& rhs);
    Dir& operator=(const unsigned& rhs);
    
    Dir& opposite();
    Dir& turnLeft();
    Dir& turnRight();
    
    bool operator==(const Dir&rhs);
    bool operator!=(const Dir&rhs);

    bool contains(const Dir& rhs) const;
    bool uncross(const Dir& rhs) const;
    
    unsigned getRaw() const;
    void reset();
    
    static Dir rand();
    
private:
    unsigned value;
};

bool operator==(const Dir& lhs, const Dir& rhs);
bool operator!=(const Dir& lhs, const Dir& rhs);