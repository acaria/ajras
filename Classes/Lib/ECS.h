#pragma once

// Kult - A lightweight entity/component library written in C++11
// rlyeh, 2013 - 2014. MIT licensed.

#include <map>
#include <set>

namespace ecs
{
    enum {JOIN,MERGE,EXCLUDE};
    
    template<typename T,int>
    struct component
    {
        T value_type;
    };
    
    template<typename T>
    std::set<unsigned> &system(unsigned group)
    {
        static std::map<unsigned, std::set<unsigned>> entities;
        if (entities.find(group) == entities.end())
            entities[group] = std::set<unsigned>();
        return entities[group];
    }
    
    template<typename T>
    void del2System(unsigned id, unsigned g)
    {
        system<T>(g).erase(id);
    }

    template<typename T>
    void add2System(unsigned id, unsigned g)
    {
        system<T>(g).insert(id);
    }

    
    template<typename T,int MODE>
    std::set<unsigned> subsystem(const std::set<unsigned> &B, unsigned group)
    {
        std::set<unsigned> newset;
        const std::set<unsigned> &A = system<T>(group);
        if (MODE==MERGE)
        {
            newset=B;
            for(auto&id:A)
                newset.insert(id);
        }
        else if(MODE==EXCLUDE)
        {
            newset=B;
            for(auto&id:A)
                newset.erase(id);
        }
        else if(A.size() < B.size())
        {
            for(auto &id : A)
                if (B.find(id)!=B.end())
                    newset.insert(id);
        }
        else
        {
            for(auto &id : B)
                if (A.find(id)!=A.end())
                    newset.insert(id);
        }
        return newset;
    }
    
    template<typename T>
    std::map<unsigned,T> &components()
    {
        static std::map<unsigned,T>objects;
        return objects;
    }
    
    template<typename T>
    bool has(unsigned id)
    {
        return components<T>().find(id) != components<T>().end();
    }
    
    template<typename T>
    void move(unsigned id, unsigned g1, unsigned g2)
    {
        if (has<T>(id))
        {
            system<T>(g1).erase(id);
            system<T>(g2).insert(id);
        }
    }
    
    template<typename T>
    decltype(T::value_type) &get(unsigned id)
    {
        return components<T>()[id].value_type;
    }
    
    template<typename T>
    decltype(T::value_type) &add(unsigned id, unsigned group)
    {
        system<T>(group).insert(id);
        components<T>()[id] = components<T>()[id];
        return get<T>(id);
    }
    
    template<typename T>
    bool del(unsigned id, unsigned group)
    {
        add<T>(id, group);
        components<T>().erase(id);
        system<T>(group).erase(id);
        return !has<T>(id);
    }
    
    template<typename T>
    void clearGroup(unsigned group)
    {
        system<T>(group).clear();
    }
    
    template<typename T>
    void clearComponents()
    {
        components<T>().clear();
    }
    
    template<class T, class U>
    std::set< unsigned > join(unsigned group)
    {
        return subsystem<T,JOIN>( system<U>(group), group);
    }
    
    template<class T, class U, class V>
    std::set< unsigned > join(unsigned group)
    {
        return subsystem<T,JOIN>( join<U,V>(group), group);
    }
    
    template<class T, class U, class V, class W>
    std::set< unsigned > join(unsigned group)
    {
        return subsystem<T,JOIN>( join<U,V,W>(group) );
    }
    
    template<class T, class U, class V, class W, class X>
    std::set< unsigned > join(unsigned group)
    {
        return subsystem<T,JOIN>( join<U,V,W,X>(group) );
    }
    
    template<class T>
    std::set< unsigned > exclude(const std::set<unsigned> &B, unsigned group)
    {
        return subsystem<T,EXCLUDE>(B, group);
    }
    
    template<class T, class U>
    bool has(unsigned id)
    {
        return has<T>(id) && has<U>(id);
    }
    
    template<class T, class U, class V>
    bool has(unsigned id)
    {
        return has<T>(id) && has<U, V>(id);
    }
    
    template<class T, class U, class V, class W>
    bool has(unsigned id)
    {
        return has<T>(id) && has<U,V,W>(id);
    }    
};