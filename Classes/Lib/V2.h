#pragma once
#include <cmath>

namespace lib
{

template <class T>
class v2
{
public:
    T x, y;
    
    v2() :x(0), y(0) {}
    v2(T x, T y) : x(x), y(y) {}
    v2(const v2& v) : x(v.x), y(v.y) {}
    
    v2& operator=(const v2& v) {
        x = v.x;
        y = v.y;
        return *this;
    }
    
    v2 operator+(v2& v) {
        return v2(x + v.x, y + v.y);
    }
    
    v2 operator-(v2& v) {
        return v2(x - v.x, y - v.y);
    }
    
    v2& operator+=(v2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    
    v2& operator-=(v2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    
    v2 operator+(double s) {
        return v2(x + s, y + s);
    }
    
    v2 operator-(double s) {
        return v2(x - s, y - s);
    }
    
    v2 operator*(double s) {
        return v2(x * s, y * s);
    }
    
    v2 operator/(double s) {
        return v2(x / s, y / s);
    }
    
    v2& operator+=(double s) {
        x += s;
        y += s;
        return *this;
    }
    
    v2& operator-=(double s) {
        x -= s;
        y -= s;
        return *this;
    }
    
    v2& operator*=(double s) {
        x *= s;
        y *= s;
        return *this;
    }
    
    v2& operator/=(double s) {
        x /= s;
        y /= s;
        return *this;
    }
    
    void set(T x, T y) {
        this->x = x;
        this->y = y;
    }
    
    void rotate(double deg) {
        double theta = deg / 180.0 * M_PI;
        double c = cos(theta);
        double s = sin(theta);
        double tx = x * c - y * s;
        double ty = x * s + y * c;
        x = tx;
        y = ty;
    }
    
    bool isZero() {
        return this->x == 0 && this->y == 0;
    }
    
    bool operator==(v2& v1)
    {
        return v1.x == x && v1.y == y;
    }
    
    v2& normalize() {
        if (length() == 0) return *this;
        *this *= (1.0 / length());
        return *this;
    }
    
    float dist(v2 v) const {
        v2 d(v.x - x, v.y - y);
        return d.length();
    }
    float length() const {
        return std::sqrt(x * x + y * y);
    }
    void truncate(double length) {
        double angle = atan2f(y, x);
        x = length * cos(angle);
        y = length * sin(angle);
    }
    
    v2 ortho() const {
        return v2(y, -x);
    }
    
    static float dot(v2 vv1, v2 vv2) {
        return vv1.x * vv2.x + vv1.y * vv2.y;
    }
    static float cross(v2 vv1, v2 vv2) {
        return (vv1.x * vv2.y) - (vv1.y * vv2.x);
    }
};

template <class T>
bool operator==(const v2<T>& v1, const v2<T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

typedef v2<float> v2f;
typedef v2<double> v2d;
typedef v2<int> v2i;
typedef v2<unsigned> v2u;
}