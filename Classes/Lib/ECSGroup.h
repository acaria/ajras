#pragma once
#include "ECS.h"

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
    ecs::set<unsigned> &system()
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
    ecs::set<unsigned> subsystem(const ecs::set<unsigned> &B)
    {
        return subsystem<T, MODE>(B, this->gid);
    }
    
    template<class T, class U>
    ecs::set< unsigned > join()
    {
        return ecs::subsystem<T,ecs::JOIN>( system<U>(), this->gid);
    }
    
    template<class T, class U, class V>
    ecs::set< unsigned > join()
    {
        return ecs::subsystem<T,ecs::JOIN>( join<U,V>(), this->gid);
    }
    
    template<class T, class U, class V, class W>
    ecs::set< unsigned > join()
    {
        return ecs::subsystem<T,ecs::JOIN>( join<U,V,W>(), this->gid);
    }
    
    template<class T, class U, class V, class W, class X>
    ecs::set< unsigned > join()
    {
        return ecs::subsystem<T,ecs::JOIN>( join<U,V,W,X>(), this->gid);
    }

    template<class T, class U, class V, class W, class X, class Y>
    ecs::set< unsigned > join()
    {
        return ecs::subsystem<T,ecs::JOIN>( join<U,V,W,X,Y>(), this->gid);
    }
    
    template<class T, class U, class V, class W, class X, class Y, class Z>
    ecs::set< unsigned > join()
    {
        return ecs::subsystem<T,ecs::JOIN>( join<U,V,W,X,Y,Z>(), this->gid);
    }
    
    template<class T>
    ecs::set< unsigned > exclude(const ecs::set<unsigned> &B)
    {
        return subsystem<T,ecs::EXCLUDE>(B, this->gid);
    }
    
private:
    unsigned gid;
};
}
