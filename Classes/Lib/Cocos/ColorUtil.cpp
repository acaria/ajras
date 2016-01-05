#include "ColorUtil.h"

cc::Color3B ColorUtil::interpolate(const cc::Color3B& start, const cc::Color3B& end, float value)
{
    GLubyte h1, s1, v1;
    GLubyte h2, s2, v2;
    
    rgb2hsv(start.r, start.g, start.b, h1, s1, v1);
    rgb2hsv(end.r, end.g, end.b, h2, s2, v2);
    
    cc::Color3B result;
    
    hsv2rgb(linear(h1, h2, value),
            linear(s1, s2, value),
            linear(v1, v2, value),
            result.r, result.g, result.b);
    
    return result;
}

GLubyte ColorUtil::linear(const GLubyte& v1, const GLubyte& v2, float amount)
{
    return v1 * (1 - amount) + v2 * amount;
}

void ColorUtil::rgb2hsv(const GLubyte &src_r, const GLubyte &src_g, const GLubyte &src_b,
                        GLubyte &dst_h, GLubyte &dst_s, GLubyte &dst_v)
{
    float r = src_r / 255.0f;
    float g = src_g / 255.0f;
    float b = src_b / 255.0f;
    
    float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0
    
    float max = fmaxf(r, fmaxf(g, b));
    float min = fminf(r, fminf(g, b));
    
    v = max;
    
    if (max == 0.0f) {
        s = 0;
        h = 0;
    }
    else if (max - min == 0.0f) {
        s = 0;
        h = 0;
    }
    else {
        s = (max - min) / max;
        
        if (max == r) {
            h = 60 * ((g - b) / (max - min)) + 0;
        }
        else if (max == g) {
            h = 60 * ((b - r) / (max - min)) + 120;
        }
        else {
            h = 60 * ((r - g) / (max - min)) + 240;
        }
    }
    
    if (h < 0) h += 360.0f;
    
    dst_h = (GLubyte)(h / 2);   // dst_h : 0-180
    dst_s = (GLubyte)(s * 255); // dst_s : 0-255
    dst_v = (GLubyte)(v * 255); // dst_v : 0-255
}

void ColorUtil::hsv2rgb(const GLubyte &src_h, const GLubyte &src_s, const GLubyte &src_v,
                        GLubyte &dst_r, GLubyte &dst_g, GLubyte &dst_b)
{
    float h = src_h *   2.0f; // 0-360
    float s = src_s / 255.0f; // 0.0-1.0
    float v = src_v / 255.0f; // 0.0-1.0
    
    float r, g, b; // 0.0-1.0
    
    int   hi = (int)(h / 60.0f) % 6;
    float f  = (h / 60.0f) - hi;
    float p  = v * (1.0f - s);
    float q  = v * (1.0f - s * f);
    float t  = v * (1.0f - s * (1.0f - f));
    
    switch(hi) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
    
    dst_r = (GLubyte)(r * 255); // dst_r : 0-255
    dst_g = (GLubyte)(g * 255); // dst_r : 0-255
    dst_b = (GLubyte)(b * 255); // dst_r : 0-255
}
