#include "ValueEx.h"

#include <sstream>
#include <iomanip>

namespace lib {

const ValueExVector ValueVectorNull;
const ValueExMap ValueMapNull;
const ValueExMapIntKey ValueMapIntKeyNull;

const ValueEx ValueEx::Null;

ValueEx::ValueEx()
: _type(Type::NONE)
{
    memset(&_field, 0, sizeof(_field));
}

ValueEx::ValueEx(unsigned char v)
: _type(Type::BYTE)
{
    _field.byteVal = v;
}

ValueEx::ValueEx(int v)
: _type(Type::INTEGER)
{
    _field.intVal = v;
}

ValueEx::ValueEx(float v)
: _type(Type::FLOAT)
{
    _field.floatVal = v;
}

ValueEx::ValueEx(double v)
: _type(Type::DOUBLE)
{
    _field.doubleVal = v;
}

ValueEx::ValueEx(bool v)
: _type(Type::BOOLEAN)
{
    _field.boolVal = v;
}

ValueEx::ValueEx(const char* v)
: _type(Type::STRING)
{
    _field.strVal = new std::string();
    if (v)
    {
        *_field.strVal = v;
    }
}

ValueEx::ValueEx(const std::string& v)
: _type(Type::STRING)
{
    _field.strVal = new std::string();
    *_field.strVal = v;
}

ValueEx::ValueEx(const ValueExVector& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) ValueExVector();
    *_field.vectorVal = v;
}

ValueEx::ValueEx(ValueExVector&& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) ValueExVector();
    *_field.vectorVal = std::move(v);
}

ValueEx::ValueEx(const ValueExMap& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) ValueExMap();
    *_field.mapVal = v;
}

ValueEx::ValueEx(ValueExMap&& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) ValueExMap();
    *_field.mapVal = std::move(v);
}

ValueEx::ValueEx(const ValueExMapIntKey& v)
: _type(Type::INT_KEY_MAP)
{
    _field.intKeyMapVal = new (std::nothrow) ValueExMapIntKey();
    *_field.intKeyMapVal = v;
}

ValueEx::ValueEx(ValueExMapIntKey&& v)
: _type(Type::INT_KEY_MAP)
{
    _field.intKeyMapVal = new (std::nothrow) ValueExMapIntKey();
    *_field.intKeyMapVal = std::move(v);
}

ValueEx::ValueEx(const cc::Rect& r) : _type(Type::RECT)
{
    _field.rectVal = new cc::Rect();
    *_field.rectVal = r;
}

ValueEx::ValueEx(cc::Rect&& r) : _type(Type::RECT)
{
    _field.rectVal = new cc::Rect();
    *_field.rectVal = std::move(r);
}

ValueEx::ValueEx(const cc::Point& p) : _type(Type::POINT)
{
    _field.pointVal = new cc::Point();
    *_field.pointVal = p;
}
   
ValueEx::ValueEx(cc::Point&& p) : _type(Type::POINT)
{
    _field.pointVal = new cc::Point();
    *_field.pointVal = std::move(p);
}

ValueEx::ValueEx(const lib::v2u& coord) : _type(Type::COORD)
{
    _field.coordVal = new lib::v2u();
    *_field.coordVal = coord;
}

ValueEx::ValueEx(lib::v2u&& coord) : _type(Type::COORD)
{
    _field.coordVal = new lib::v2u();
    *_field.coordVal = std::move(coord);
}

ValueEx::ValueEx(const ValueEx& other) : _type(Type::NONE)
{
    *this = other;
}

ValueEx::ValueEx(ValueEx&& other) : _type(Type::NONE)
{
    *this = std::move(other);
}

ValueEx::~ValueEx()
{
    clear();
}

ValueEx& ValueEx::operator= (const ValueEx& other)
{
    if (this != &other) {
        reset(other._type);
        
        switch (other._type) {
            case Type::BYTE:
                _field.byteVal = other._field.byteVal;
                break;
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                if (_field.strVal == nullptr)
                {
                    _field.strVal = new std::string();
                }
                *_field.strVal = *other._field.strVal;
                break;
            case Type::VECTOR:
                if (_field.vectorVal == nullptr)
                {
                    _field.vectorVal = new (std::nothrow) ValueExVector();
                }
                *_field.vectorVal = *other._field.vectorVal;
                break;
            case Type::MAP:
                if (_field.mapVal == nullptr)
                {
                    _field.mapVal = new (std::nothrow) ValueExMap();
                }
                *_field.mapVal = *other._field.mapVal;
                break;
            case Type::INT_KEY_MAP:
                if (_field.intKeyMapVal == nullptr)
                {
                    _field.intKeyMapVal = new (std::nothrow) ValueExMapIntKey();
                }
                *_field.intKeyMapVal = *other._field.intKeyMapVal;
                break;
            case Type::POINT:
                if (_field.pointVal == nullptr)
                {
                    _field.pointVal = new cc::Point();
                }
                *_field.pointVal = *other._field.pointVal;
                break;
            case Type::COORD:
                if (_field.coordVal == nullptr)
                {
                    _field.coordVal = new lib::v2u();
                }
                *_field.coordVal = *other._field.coordVal;
                break;
            case Type::RECT:
                if (_field.rectVal == nullptr)
                {
                    _field.rectVal = new cc::Rect();
                }
                *_field.rectVal = *other._field.rectVal;
                break;
            default:
                break;
        }
    }
    return *this;
}

ValueEx& ValueEx::operator= (ValueEx&& other)
{
    if (this != &other)
    {
        clear();
        switch (other._type)
        {
            case Type::BYTE:
                _field.byteVal = other._field.byteVal;
                break;
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                _field.strVal = other._field.strVal;
                break;
            case Type::VECTOR:
                _field.vectorVal = other._field.vectorVal;
                break;
            case Type::MAP:
                _field.mapVal = other._field.mapVal;
                break;
            case Type::INT_KEY_MAP:
                _field.intKeyMapVal = other._field.intKeyMapVal;
                break;
            case Type::POINT:
                _field.pointVal = other._field.pointVal;
                break;
            case Type::COORD:
                _field.coordVal = other._field.coordVal;
                break;
            case Type::RECT:
                _field.rectVal = other._field.rectVal;
                break;
            default:
                break;
        }
        _type = other._type;
        
        memset(&other._field, 0, sizeof(other._field));
        other._type = Type::NONE;
    }
    
    return *this;
}

ValueEx& ValueEx::operator= (unsigned char v)
{
    reset(Type::BYTE);
    _field.byteVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (int v)
{
    reset(Type::INTEGER);
    _field.intVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (float v)
{
    reset(Type::FLOAT);
    _field.floatVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (double v)
{
    reset(Type::DOUBLE);
    _field.doubleVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (bool v)
{
    reset(Type::BOOLEAN);
    _field.boolVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (const char* v)
{
    reset(Type::STRING);
    *_field.strVal = v ? v : "";
    return *this;
}

ValueEx& ValueEx::operator= (const std::string& v)
{
    reset(Type::STRING);
    *_field.strVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (const ValueExVector& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (ValueExVector&& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = std::move(v);
    return *this;
}

ValueEx& ValueEx::operator= (const ValueExMap& v)
{
    reset(Type::MAP);
    *_field.mapVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (ValueExMap&& v)
{
    reset(Type::MAP);
    *_field.mapVal = std::move(v);
    return *this;
}

ValueEx& ValueEx::operator= (const ValueExMapIntKey& v)
{
    reset(Type::INT_KEY_MAP);
    *_field.intKeyMapVal = v;
    return *this;
}

ValueEx& ValueEx::operator= (ValueExMapIntKey&& v)
{
    reset(Type::INT_KEY_MAP);
    *_field.intKeyMapVal = std::move(v);
    return *this;
}

ValueEx& ValueEx::operator= (const cc::Point& p)
{
    reset(Type::POINT);
    *_field.pointVal = p;
    return *this;
}

ValueEx& ValueEx::operator= (cc::Point&& p)
{
    reset(Type::POINT);
    *_field.pointVal = std::move(p);
    return *this;
}

ValueEx& ValueEx::operator= (const cc::Rect& r)
{
    reset(Type::RECT);
    *_field.rectVal = r;
    return *this;
}

ValueEx& ValueEx::operator= (cc::Rect&& r)
{
    reset(Type::RECT);
    *_field.rectVal = std::move(r);
    return *this;
}

ValueEx& ValueEx::operator= (const lib::v2u& c)
{
    reset(Type::COORD);
    *_field.coordVal = c;
    return *this;
}

ValueEx& ValueEx::operator= (lib::v2u&& c)
{
    reset(Type::COORD);
    *_field.coordVal = std::move(c);
    return *this;
}

bool ValueEx::operator!= (const ValueEx& v)
{
    return !(*this == v);
}
bool ValueEx::operator!= (const ValueEx& v) const
{
    return !(*this == v);
}

bool ValueEx::operator== (const ValueEx& v)
{
    const auto &t = *this;
    return t == v;
}
bool ValueEx::operator== (const ValueEx& v) const
{
    if (this == &v) return true;
    if (v._type != this->_type) return false;
    if (this->isNull()) return true;
    switch (_type)
    {
        case Type::BYTE:    return v._field.byteVal   == this->_field.byteVal;
        case Type::INTEGER: return v._field.intVal    == this->_field.intVal;
        case Type::BOOLEAN: return v._field.boolVal   == this->_field.boolVal;
        case Type::STRING:  return *v._field.strVal   == *this->_field.strVal;
        case Type::FLOAT:   return fabs(v._field.floatVal  - this->_field.floatVal)  <= FLT_EPSILON;
        case Type::DOUBLE:  return fabs(v._field.doubleVal - this->_field.doubleVal) <= FLT_EPSILON;
        case Type::COORD: return *v._field.coordVal == *this->_field.coordVal;
        case Type::RECT: return v._field.rectVal->origin == this->_field.rectVal->origin &&
                                v._field.rectVal->size.width == this->_field.rectVal->size.width &&
                                v._field.rectVal->size.height == this->_field.rectVal->size.height;
        case Type::POINT: return *v._field.pointVal == *this->_field.pointVal;
        case Type::VECTOR:
        {
            const auto &v1 = *(this->_field.vectorVal);
            const auto &v2 = *(v._field.vectorVal);
            const auto size = v1.size();
            if (size == v2.size())
            {
                for (size_t i = 0; i < size; i++)
                {
                    if (v1[i] != v2[i]) return false;
                }
            }
            return true;
        }
        case Type::MAP:
        {
            const auto &map1 = *(this->_field.mapVal);
            const auto &map2 = *(v._field.mapVal);
            for (const auto &kvp : map1)
            {
                auto it = map2.find(kvp.first);
                if (it == map2.end() || it->second != kvp.second)
                {
                    return false;
                }
            }
            return true;
        }
        case Type::INT_KEY_MAP:
        {
            const auto &map1 = *(this->_field.intKeyMapVal);
            const auto &map2 = *(v._field.intKeyMapVal);
            for (const auto &kvp : map1)
            {
                auto it = map2.find(kvp.first);
                if (it == map2.end() || it->second != kvp.second)
                {
                    return false;
                }
            }
            return true;
        }
        default:
            break;
    };
    
    return false;
}

/// Convert value to a specified type
unsigned char ValueEx::asByte() const
{
    CCASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    
    if (_type == Type::BYTE)
    {
        return _field.byteVal;
    }
    
    if (_type == Type::INTEGER)
    {
        return static_cast<unsigned char>(_field.intVal);
    }
    
    if (_type == Type::STRING)
    {
        return static_cast<unsigned char>(atoi(_field.strVal->c_str()));
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<unsigned char>(_field.floatVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<unsigned char>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }
    
    return 0;
}

int ValueEx::asInt() const
{
    CCASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::INTEGER)
    {
        return _field.intVal;
    }
    
    if (_type == Type::BYTE)
    {
        return _field.byteVal;
    }
    
    if (_type == Type::STRING)
    {
        return atoi(_field.strVal->c_str());
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<int>(_field.floatVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<int>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }
    
    return 0;
}

float ValueEx::asFloat() const
{
    CCASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::FLOAT)
    {
        return _field.floatVal;
    }
    
    if (_type == Type::BYTE)
    {
        return static_cast<float>(_field.byteVal);
    }
    
    if (_type == Type::STRING)
    {
        return std::stof(*_field.strVal);
    }
    
    if (_type == Type::INTEGER)
    {
        return static_cast<float>(_field.intVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<float>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0f : 0.0f;
    }
    
    return 0.0f;
}

double ValueEx::asDouble() const
{
    CCASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal;
    }
    
    if (_type == Type::BYTE)
    {
        return static_cast<double>(_field.byteVal);
    }
    
    if (_type == Type::STRING)
    {
        return std::stod(*_field.strVal);
    }
    
    if (_type == Type::INTEGER)
    {
        return static_cast<double>(_field.intVal);
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<double>(_field.floatVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0 : 0.0;
    }
    
    return 0.0;
}

bool ValueEx::asBool() const
{
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal;
    }
    
    if (_type == Type::BYTE)
    {
        return _field.byteVal == 0 ? false : true;
    }
    
    if (_type == Type::STRING)
    {
        return (*_field.strVal == "0" || *_field.strVal == "false") ? false : true;
    }
    
    if (_type == Type::INTEGER)
    {
        return _field.intVal == 0 ? false : true;
    }
    
    if (_type == Type::FLOAT)
    {
        return _field.floatVal == 0.0f ? false : true;
    }
    
    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal == 0.0 ? false : true;
    }
    
    return false;
}

std::string ValueEx::asString() const
{
    CCASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    
    if (_type == Type::STRING)
    {
        return *_field.strVal;
    }
    
    std::stringstream ret;
    
    switch (_type)
    {
        case Type::BYTE:
            ret << _field.byteVal;
            break;
        case Type::INTEGER:
            ret << _field.intVal;
            break;
        case Type::FLOAT:
            ret << std::fixed << std::setprecision( 7 )<< _field.floatVal;
            break;
        case Type::DOUBLE:
            ret << std::fixed << std::setprecision( 16 ) << _field.doubleVal;
            break;
        case Type::BOOLEAN:
            ret << (_field.boolVal ? "true" : "false");
            break;
        default:
            break;
    }
    return ret.str();
}

cc::Rect& ValueEx::asRect()
{
    CCASSERT(_type == Type::RECT, "The value type isn't Type::RECT");
    return *_field.rectVal;
}
    
const cc::Rect& ValueEx::asRect() const
{
    CCASSERT(_type == Type::RECT, "The value type isn't Type::RECT");
    return *_field.rectVal;
}

cc::Point& ValueEx::asPoint()
{
    CCASSERT(_type == Type::POINT, "The value type isn't Type::POINT");
    return *_field.pointVal;
}
    
const cc::Point& ValueEx::asPoint() const
{
    CCASSERT(_type == Type::POINT, "The value type isn't Type::POINT");
    return *_field.pointVal;
}
    
lib::v2u& ValueEx::asCoord()
{
    CCASSERT(_type == Type::COORD, "The value type isn't Type::COORD");
    return *_field.coordVal;
}
    
const lib::v2u& ValueEx::asCoord() const
{
    CCASSERT(_type == Type::COORD, "The value type isn't Type::COORD");
    return *_field.coordVal;
}

ValueExVector& ValueEx::asValueExVector()
{
    CCASSERT(_type == Type::VECTOR, "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

const ValueExVector& ValueEx::asValueExVector() const
{
    CCASSERT(_type == Type::VECTOR, "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

ValueExMap& ValueEx::asValueMap()
{
    CCASSERT(_type == Type::MAP, "The value type isn't Type::MAP");
    return *_field.mapVal;
}

const ValueExMap& ValueEx::asValueExMap() const
{
    CCASSERT(_type == Type::MAP, "The value type isn't Type::MAP");
    return *_field.mapVal;
}

ValueExMapIntKey& ValueEx::asIntKeyMap()
{
    CCASSERT(_type == Type::INT_KEY_MAP, "The value type isn't Type::INT_KEY_MAP");
    return *_field.intKeyMapVal;
}

const ValueExMapIntKey& ValueEx::asIntKeyMap() const
{
    CCASSERT(_type == Type::INT_KEY_MAP, "The value type isn't Type::INT_KEY_MAP");
    return *_field.intKeyMapVal;
}

void ValueEx::clear()
{
    // Free memory the old value allocated
    switch (_type)
    {
        case Type::BYTE:
            _field.byteVal = 0;
            break;
        case Type::INTEGER:
            _field.intVal = 0;
            break;
        case Type::FLOAT:
            _field.floatVal = 0.0f;
            break;
        case Type::DOUBLE:
            _field.doubleVal = 0.0;
            break;
        case Type::BOOLEAN:
            _field.boolVal = false;
            break;
        case Type::STRING:
            CC_SAFE_DELETE(_field.strVal);
            break;
        case Type::VECTOR:
            CC_SAFE_DELETE(_field.vectorVal);
            break;
        case Type::MAP:
            CC_SAFE_DELETE(_field.mapVal);
            break;
        case Type::INT_KEY_MAP:
            CC_SAFE_DELETE(_field.intKeyMapVal);
            break;
        case Type::POINT:
            CC_SAFE_DELETE(_field.pointVal);
            break;
        case Type::RECT:
            CC_SAFE_DELETE(_field.rectVal);
            break;
        case Type::COORD:
            CC_SAFE_DELETE(_field.coordVal);
            break;
        default:
            break;
    }
    
    _type = Type::NONE;
}

void ValueEx::reset(Type type)
{
    if (_type == type)
        return;
    
    clear();
    
    // Allocate memory for the new value
    switch (type)
    {
        case Type::STRING:
            _field.strVal = new std::string();
            break;
        case Type::VECTOR:
            _field.vectorVal = new (std::nothrow) ValueExVector();
            break;
        case Type::MAP:
            _field.mapVal = new (std::nothrow) ValueExMap();
            break;
        case Type::INT_KEY_MAP:
            _field.intKeyMapVal = new (std::nothrow) ValueExMapIntKey();
            break;
        case Type::COORD:
            _field.coordVal = new lib::v2u();
            break;
        case Type::POINT:
            _field.pointVal = new cc::Point();
            break;
        case Type::RECT:
            _field.rectVal = new cc::Rect();
            break;
        default:
            break;
    }
    
    _type = type;
}

}//namespace lib