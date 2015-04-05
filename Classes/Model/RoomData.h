#pragma once
#include "Headers.h"

class RoomData
{
public:
    RoomData(unsigned roomIndex, RoomModel* model) : model(model),
                                                     index(roomIndex)
    {
        for(auto gate : model->gates)
            gateMapping[gate.first] = {0,0};
    }
    
    RoomModel* getModel()
    {
        return model;
    }
    
    cocos2d::Rect getBounds()
    {
        return cocos2d::Rect(position.x,
                             position.y,
                             model->totalSize.x,
                             model->totalSize.y);
    }
    
    unsigned                                            index;
    cocos2d::Vec2                                       position;
    
    //mapping src_gateindex => dest_roomindex + dest_gateindex
    std::map<unsigned, std::pair<unsigned, unsigned>>   gateMapping;
    
private:
    RoomModel*                                          model;
};