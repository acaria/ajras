#pragma once
#include "Headers.h"

namespace lib
{
class EcsGroup
{
public:
    EcsGroup() : gid(0) {}
    EcsGroup(unsigned id) : gid(id) {}
    void setID(unsigned id) { gid = id; }
    unsigned getID() { return gid; }
    
    template<typename T>
    decltype(T::value_type) &add(unsigned id)
    {
        return ecs::add<T>(id, this->gid);
    }
    
    template<typename T>
    std::set<unsigned> &system()
    {
        return ecs::system<T>(this->gid);
    }
    
    template<typename T>
    bool del(unsigned id)
    {
        return ecs::del<T>(id, this->gid);
    }
    
    template<typename T>
    void clear()
    {
        clear<T>(this->gid);
    }
    
    template<typename T,int MODE>
    std::set<unsigned> subsystem(const std::set<unsigned> &B)
    {
        return subsystem<T, MODE>(B, this->gid);
    }
    
    template<class T, class U>
    std::set< unsigned > join()
    {
        return ecs::subsystem<T,ecs::JOIN>( system<U>(), this->gid);
    }
    
    template<class T, class U, class V>
    std::set< unsigned > join()
    {
        return ecs::subsystem<T,ecs::JOIN>( join<U,V>(), this->gid);
    }
    
    template<class T, class U, class V, class W>
    std::set< unsigned > join()
    {
        return subsystem<T,ecs::JOIN>( join<U,V,W>(this->gid) );
    }
    
    template<class T>
    std::set< unsigned > exclude(const std::set<unsigned> &B)
    {
        return subsystem<T,ecs::EXCLUDE>(B, this->gid);
    }
    
private:
    unsigned gid;
};
}
