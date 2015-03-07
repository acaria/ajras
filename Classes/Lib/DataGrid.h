#pragma once
#include "Headers.h"

#include <iostream>
#include <unordered_map>
#include <functional>
#include <vector>

namespace lib {

template<typename VALUE>
class DataGrid
{
public:
    DataGrid(const DataGrid& r) :   width(r.width),
                                    height(r.height),
                                    content(r.content)
    {}
    
    DataGrid& operator=(const DataGrid& r) {
        width = r.width;
        height = r.height;
        content = r.content;
        
        return *this;
    }


    DataGrid(unsigned width, unsigned height):
    width(width),
    height(height),
    content(width * height, [width](const v2u &pos){
        return pos.y * width + pos.x;
    }) {}
    
    VALUE &get(unsigned x, unsigned y)
    {
        return content[{x, y}];
    }
    
    VALUE &get(const v2u &pos)
    {
        return content[pos];
    }
    
    std::vector<VALUE> getValues()
    {
        std::vector<VALUE> list;
        list.reserve(content.size());
        for(auto kv : content)
        {
            list.push_back(kv.second);
        }
        return list;
    }
    
    std::vector<v2u> getKeys()
    {
        std::vector<v2u> list;
        list.reserve(content.size());
        for(auto kv : content)
        {
            list.push_back(kv.first);
        }
        return list;
    }
    
    
    VALUE &operator[](const v2u &pos)
    {
        return this->get(pos);
    }
    
    bool has(unsigned x, unsigned y)
    {
        return content.find({x,y}) != content.end();
    }
    
    bool has(const v2u &pos)
    {
        return content.find(pos) != content.end();
    }
    
    void remove(const v2u &pos)
    {
        content.erase(pos);
    }
    
    void clear()
    {
        content.clear();
    }
    
    unsigned width, height;
    
private:
    std::unordered_map<v2u, VALUE, std::function<size_t(const v2u &pos)>> content;
};

}