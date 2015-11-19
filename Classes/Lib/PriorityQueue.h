#pragma once

#include <PriorityQueue.h>

template<typename T, typename Number=int>
struct PriorityQueue
{
    typedef std::pair<Number, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;
    
    inline bool empty()
    {
        return elements.empty();
    }
    
    inline void put(T item, Number priority)
    {
        elements.emplace(priority, item);
    }
    
    inline T get()
    {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};