#pragma once

namespace cc = cocos2d;

#ifdef NDEBUG
#define Log(...) do {} while (0)
#else
#define Log(...) cocos2d::log(__VA_ARGS__)
#endif