#pragma once

#include "RoomModel.h"

class RoomData
{
public:
    enum class RoomType
    {
        START,
        COMMON,
        END
    };

    RoomData(unsigned roomIndex, RoomModel* model) : model(model),
                                                     index(roomIndex)
    {
        this->type = RoomType::COMMON;
        this->depth = 0;
        for(auto gate : model->gates)
            gateMapping[gate.first] = GateMap(0,0);
    }
    
    RoomModel* getModel()
    {
        return model;
    }
    
    cc::Rect getBounds()
    {
        return cocos2d::Rect(position.x,
                             position.y,
                             model->totalSize.x,
                             model->totalSize.y);
    }
    
    unsigned        index;
    unsigned        depth;
    cocos2d::Vec2   position;
    RoomType        type;
    
    
    struct GateMap
    {
        GateMap(){};
        
        GateMap(unsigned roomIndex, unsigned gateIndex)
        {
            this->roomIndex = roomIndex;
            this->gateIndex = gateIndex;
        }
        
        unsigned roomIndex;
        unsigned gateIndex;
    };
    
    //mapping src_gateindex => dest_roomindex + dest_gateindex
    std::map<unsigned, GateMap>   gateMapping;
    
private:
    RoomModel*                                          model;
};