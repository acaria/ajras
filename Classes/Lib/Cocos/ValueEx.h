#pragma once


#include <string>
#include <vector>
#include <unordered_map>

#include "CoreLib.h"

namespace  lib {

class ValueEx;

typedef std::vector<ValueEx> ValueExVector;
typedef std::unordered_map<std::string, ValueEx> ValueExMap;
typedef std::unordered_map<int, ValueEx> ValueExMapIntKey;

extern const ValueExVector ValueExVectorNull;
extern const ValueExMap ValueExMapNull;
extern const ValueExMapIntKey ValueExMapIntKeyNull;

class ValueEx
{
public:
    /** A predefined Value that has not value. */
    static const ValueEx Null;
    
    //ctors
    ValueEx();
    explicit ValueEx(unsigned char v);
    explicit ValueEx(int v);
    explicit ValueEx(float v);
    explicit ValueEx(double v);
    explicit ValueEx(bool v);
    explicit ValueEx(const char* v);
    explicit ValueEx(const std::string& v);
    explicit ValueEx(const ValueExVector& v);
    explicit ValueEx(ValueExVector&& v);
    explicit ValueEx(const ValueExMap& v);
    explicit ValueEx(ValueExMap&& v);
    explicit ValueEx(const ValueExMapIntKey& v);
    explicit ValueEx(ValueExMapIntKey&& v);
    
    explicit ValueEx(const cc::Rect& r);
    explicit ValueEx(cc::Rect&& r);
    explicit ValueEx(const cc::Point& p);
    explicit ValueEx(cc::Point&& p);
    explicit ValueEx(const lib::v2u& coord);
    explicit ValueEx(lib::v2u&& coord);
    
    //copies
    ValueEx(const ValueEx& other);
    ValueEx(ValueEx&& other);
    
    ~ValueEx();
    
    //assignements
    ValueEx& operator= (const ValueEx& other);
    ValueEx& operator= (ValueEx&& other);
    ValueEx& operator= (unsigned char v);
    ValueEx& operator= (int v);
    ValueEx& operator= (float v);
    ValueEx& operator= (double v);
    ValueEx& operator= (bool v);
    ValueEx& operator= (const char* v);
    ValueEx& operator= (const std::string& v);
    
    ValueEx& operator= (const ValueExVector& v);
    ValueEx& operator= (ValueExVector&& v);
    
    ValueEx& operator= (const ValueExMap& v);
    ValueEx& operator= (ValueExMap&& v);
    
    ValueEx& operator= (const ValueExMapIntKey& v);
    ValueEx& operator= (ValueExMapIntKey&& v);
    
    ValueEx& operator= (const cc::Point& p);
    ValueEx& operator= (cc::Point&& p);
    ValueEx& operator= (const cc::Rect& r);
    ValueEx& operator= (cc::Rect&& r);
    ValueEx& operator= (const lib::v2u& c);
    ValueEx& operator= (lib::v2u&& c);
    
    bool operator!= (const ValueEx& v);
    bool operator!= (const ValueEx& v) const;
    bool operator== (const ValueEx& v);
    bool operator== (const ValueEx& v) const;
    
    unsigned char asByte() const;
    int asInt() const;
    float asFloat() const;
    double asDouble() const;
    bool asBool() const;
    std::string asString() const;
    ValueExVector& asValueExVector();
    const ValueExVector& asValueExVector() const;
    ValueExMap& asValueMap();
    const ValueExMap& asValueExMap() const;
    cc::Point& asPoint();
    const cc::Point& asPoint() const;
    cc::Rect& asRect();
    const cc::Rect& asRect() const;
    lib::v2u& asCoord();
    const lib::v2u& asCoord() const;
    
    ValueExMapIntKey& asIntKeyMap();
    const ValueExMapIntKey& asIntKeyMap() const;
    
    inline bool isNull() const { return _type == Type::NONE; }
    
    /** Value type wrapped by Value. */
    enum class Type
    {
        NONE = 0,
        BYTE,
        INTEGER,
        FLOAT,
        DOUBLE,
        BOOLEAN,
        STRING,
        VECTOR,
        MAP,
        INT_KEY_MAP,
        POINT,
        COORD,
        RECT
    };
    
    /** Gets the value type. */
    inline Type getType() const { return _type; }
    
private:
    void clear();
    void reset(Type type);
    
    union
    {
        unsigned char       byteVal;
        int                 intVal;
        float               floatVal;
        double              doubleVal;
        bool                boolVal;
        std::string*        strVal;
        cc::Point*          pointVal;
        cc::Rect*           rectVal;
        lib::v2u*           coordVal;
        ValueExVector*      vectorVal;
        ValueExMap*         mapVal;
        ValueExMapIntKey*   intKeyMapVal;
        
    }_field;
    
    Type _type;

};//class ValueEx

}//namespace lib
