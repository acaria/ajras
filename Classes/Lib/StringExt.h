#pragma once

#include <string>

namespace lib
{
    std::string& ltrim(std::string& str);
    std::string& rtrim(std::string& str);
    std::string& trim(std::string& str);
    std::string trimCopy(std::string const& str);
    
    template<class ContainerT>
    void split(const std::string& str,
               ContainerT& tokens,
               const std::string& delimiters = " ",
               bool trimEmpty = false)
    {
        std::string::size_type pos, lastPos = 0;
        
        using value_type = typename ContainerT::value_type;
        using size_type  = typename ContainerT::size_type;
        
        while(true)
        {
            pos = str.find_first_of(delimiters, lastPos);
            if(pos == std::string::npos)
            {
                pos = str.length();
                
                if(pos != lastPos || !trimEmpty)
                    tokens.push_back(value_type(str.data()+lastPos,
                                                (size_type)pos-lastPos ));
                
                break;
            }
            else
            {
                if(pos != lastPos || !trimEmpty)
                    tokens.push_back(value_type(str.data()+lastPos,
                                                (size_type)pos-lastPos ));
            }
            
            lastPos = pos + 1;
        }
    }
    
    inline std::string lowercase(const std::string &str)
    {
        std::string lower = str;
        transform(lower.begin(),
                  lower.end(),
                  lower.begin(),
                  ::tolower);
        return lower;
    }
}