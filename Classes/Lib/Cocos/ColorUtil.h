#pragma once

class ColorUtil
{
public:
    static cc::Color3B interpolate(const cc::Color3B& start, const cc::Color3B& end, float value);
private:
    static GLubyte linear(const GLubyte& v1, const GLubyte& v2, float amount);
    static void rgb2hsv(const GLubyte &src_r, const GLubyte &src_g, const GLubyte &src_b,
                 GLubyte &dst_h, GLubyte &dst_s, GLubyte &dst_v);
    static void hsv2rgb(const GLubyte &src_h, const GLubyte &src_s, const GLubyte &src_v,
                 GLubyte &dst_r, GLubyte &dst_g, GLubyte &dst_b);
};