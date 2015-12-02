#pragma once

#include <functional>
#include <vector>

namespace lib
{
    template<typename R, typename... Args>
    class Delegate
    {
    public:
        template<typename U>
        delegate& operator += (const U &func)
        {
            funcs.push_back(std::function<R(Args...)>(func));
            return *this;
        }
    
        std::vector<R> operator () (Args... params)
        {
            std::vector<R> ret;
            for (auto f : funcs)
            {
                ret.push_back(f(params...));
            }
            return ret;
        }
    private:
        std::vector<std::function<R(Args...)>> funcs;
    };

    template<typename... Args>
    class Delegate<void, Args...>
    {
    public:
        template<typename U>
        delegate& operator += (const U &func)
        {
            funcs.push_back(std::function<void(Args...)>(func));
            return *this;
        }
    
        void operator () (Args... params)
        {
            for (auto f : funcs)
            {
                f(params...);
            }
        }
    private:
        std::vector<std::function<void(Args...)>> funcs;
    };
}