#pragma once

namespace lib {

template <typename T>
class Nullable final
{
    friend bool operator==(const Nullable<T> &op1, const Nullable<T> &op2)
    {
        if (op1.m_hasValue != op2.m_hasValue)
            return false;
        
        if (op1.m_hasValue)
            return op1.m_value == op2.m_value;
        else
            return true;
    }
    
    friend bool operator==(const Nullable<T> &op, const T &value)
    {
        if (!op.m_hasValue)
            return false;
        
        return op.m_value == value;
    }
    
    friend bool operator==(const T &value, const Nullable<T> &op)
    {
        if (!op.m_hasValue)
            return false;
        
        return op.m_value == value;
    }
    
    friend bool operator==(const Nullable<T> &op, std::nullptr_t nullpointer)
    {
        (void)nullpointer;
        if (op.m_hasValue)
            return false;
        else
            return true;
    }
    
    friend bool operator==(std::nullptr_t nullpointer, const Nullable<T> &op)
    {
        (void)nullpointer;
        if (op.m_hasValue)
            return false;
        else
            return true;
    }
    
    friend bool operator!=(const Nullable<T> &op1, const Nullable<T> &op2)
    {
        if (op1.m_hasValue != op2.m_hasValue)
            return true;
        
        if (op1.m_hasValue)
            return op1.m_value != op2.m_value;
        else
            return true;
    }
    
    friend bool operator!=(const Nullable<T> &op, const T &value)
    {
        if (!op.m_hasValue)
            return true;
        
        return op.m_value != value;
    }
    
    friend bool operator!=(const T &value, const Nullable<T> &op)
    {
        if (!op.m_hasValue)
            return true;
        
        return op.m_value != value;
    }
    
    friend bool operator!=(const Nullable<T> &op, std::nullptr_t nullpointer)
    {
        (void)nullpointer;
        if (op.m_hasValue)
            return true;
        else
            return false;
    }
    
    friend bool operator!=(std::nullptr_t nullpointer, const Nullable<T> &op)
    {
        (void)nullpointer;
        if (op.m_hasValue)
            return true;
        else
            return false;
    }
    
    
public:
    Nullable() : m_hasValue(false),
                 m_value(T()),
                 Value(m_value) {
    }
    
    Nullable(const T &value) : m_hasValue(true),
                               m_value(value),
                               Value(m_value) {
    }
    
    Nullable(std::nullptr_t nullpointer) : m_hasValue(false),
                                           m_value(T()),
                                           Value(m_value) {
        (void)nullpointer;
    }

    const Nullable<T> & operator=(const Nullable<T> &value)
    {
        m_hasValue = value.m_hasValue;
        m_value = value.m_value;
        return *this;
    }
    
    const Nullable<T> & operator=(const T &value)
    {
        m_hasValue = true;
        m_value = value;
        return *this;
    }
    
    const Nullable<T> & operator=(std::nullptr_t nullpointer)
    {
        (void)nullpointer;
        m_hasValue = false;
        m_value = T();
        return *this;
    }
    
private:
    bool m_hasValue;
    T m_value;
    
public:
    const T & Value;
    const T& get() { return Value; }
};

}