#pragma once

namespace lib
{

class Random
{
public:
    Random() = default;
    Random(std::mt19937::result_type seed) : engine(seed) {}
    
    template<typename type>
    type interval(type min, type max)
    {
        return std::uniform_int_distribution<type>{min, max}(engine);
    }
    
    int interval(int min, int max)
    {
        return std::uniform_int_distribution<int>{min, max}(engine);
    }
    
    float ratio()
    {
        return std::uniform_real_distribution<float>{0.0f, 1.0f}(engine);
    }
    
    template <typename element>
    element& select(std::list<element>& list)
    {
        assert(list.size() > 0);
        auto it = list.begin();
        int pos = interval(0, list.size() - 1);
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
        auto pos = interval(0, list.size() - 1);
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
    
    template<class bidiiter>
    bidiiter random_unique(bidiiter begin, bidiiter end, size_t num_random) {
        size_t left = std::distance(begin, end);
        while (num_random--) {
            bidiiter r = begin;
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

