#pragma once

#include <utility>

namespace cocos2d
{
    template<class T, typename... Args>
    T* create(Args&& ...args) {
        auto pRet = new T();
        if (pRet && pRet->init(std::forward<Args>(args)...))
        {
            pRet->autorelease();
            return pRet;
        }
        delete pRet;
        return nullptr;
    }
    
    template <class T>
    void unique_ptr_deleter(T *p)
    {
        auto ptr = static_cast<Ref *>(p);
        if (ptr)
            ptr->release();
    }
    
    template <class T>
    using unique_ptr = std::unique_ptr<T, decltype(*unique_ptr_deleter<T>)>;
    
    template <class T>
    unique_ptr<T> make_unique(T *ptr)
    {
        if (ptr)
            ptr->retain();
        return unique_ptr<T>(ptr, unique_ptr_deleter);
    }
}