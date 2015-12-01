#pragma once

namespace lib
{

class Random
{
public:
    Random() = default;
    Random(std::mt19937::result_type seed) : engine(seed) {}
    
    template<typename T>
    inline T interval(T min, T max)
    {
        return std::uniform_int_distribution<T>{min, max}(engine);
    }
    
    inline int interval(int min, int max)
    {
        return std::uniform_int_distribution<int>{min, max}(engine);
    }
    
    inline float interval(float min, float max)
    {
        return std::uniform_real_distribution<float>{min, max}(engine);
    }
    
    inline double interval(double min, double max)
    {
        return std::uniform_real_distribution<double>{min, max}(engine);
    }
    
    inline long double interval(long double min, long double max)
    {
        return std::uniform_real_distribution<long double>{min, max}(engine);
    }
    
    inline float ratio()
    {
        return std::uniform_real_distribution<float>{0.0f, 1.0f}(engine);
    }
    
    inline float minus1_1()
    {
        return std::uniform_real_distribution<float>{-1.0f, 1.0f}(engine);
    }
    
    template <typename element>
    element& select(std::list<element>& list)
    {
        assert(list.size() > 0);
        auto it = list.begin();
        auto pos = interval(0, list.size() - 1);
        for (int i = 0; i < pos; ++it, i++);
        return *(it);
    }
    
    template <typename element>
    element& pick(std::list<element>& list)
    {
        assert(list.size() > 0);
        auto it = list.begin();
        int pos = interval(0, list.size() - 1);
        for (int i = 0; i < pos; ++it, i++);
        list.erase(it);
        return *(it);
    }
    
    template <typename element>
    element& select(std::vector<element>& list)
    {
        assert(list.size() > 0);
        int pos = interval(0, list.size() - 1);
        return list[pos];
    }
    
    template <typename element>
    element& pick(std::vector<element>& list)
    {
        assert(list.size() > 0);
        auto pos = interval(0, list.size() - 1);
        list.erase(list.begin() + pos);
        return list[pos];
    }
    
    template<class iter>
    iter randomUnique(iter begin, iter end, size_t num_random) {
        size_t left = std::distance(begin, end);
        while (num_random--) {
            iter r = begin;
            std::advance(r, std::rand() % left);
            std::swap(*begin, *r);
            ++begin;
            --left;
        }
        return begin;
    }
    
    std::mt19937& getEngine()
    {
        return engine;
    }
    
private:
    std::mt19937 engine{std::random_device{}()};

};
}

