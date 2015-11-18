#pragma once

class IMapData;

class NavigationInfo
{
public:
    void init(IMapData* data);
    
private:
    IMapData* data;
};