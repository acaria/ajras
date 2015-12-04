#include "AIHelper.h"
#include "AISystem.h"
#include "RoomData.h"

AIHelper::AIHelper(AISystem* system) : system(system) {}

def::mood::Flags AIHelper::getMoodGroup(def::mood::Flags ref,
                                        const std::string& moodGroupCat)
{
    switch(lib::hash(moodGroupCat))
    {
        case lib::hash("opponent"): return def::mood::getOpponents(ref);
        case lib::hash("ally"): return def::mood::getAllies(ref);
        default:
            Log("unrecognised mood group selection : %s", moodGroupCat.c_str());
            break;
    }
    return def::mood::Neutral;
}

behaviour::nState AIHelper::checkNearMood(unsigned eid, float sight,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 2); //params=[category,value]
    
    auto maxDist = sight * sight;
    auto moodGroup = getMoodGroup(ecs::get<cp::Mood>(eid), params[1]);
    auto bounds = SysHelper::getBounds(eid);
    
    float nearest = maxDist;
    unsigned targetID = 0;

    for(auto tid : system->context->ecs->join<cp::AI, cp::Position, cp::Physics, cp::Mood>())
    {
        if (tid == eid)
            continue;
        if (!def::mood::inside(ecs::get<cp::Mood>(tid), moodGroup))
            continue;
        auto bounds2 = SysHelper::getBounds(tid);
        float dist = cc::Vec2(bounds.getMidX() - bounds2.getMidX(),
                              bounds.getMidY() - bounds2.getMidY()).lengthSquared();
        if (dist < maxDist && dist < nearest)
        {
            nearest = dist;
            targetID = tid;
        }
    }

    return (targetID != 0) ? state::SUCCESS : state::FAILURE;
}

behaviour::nState AIHelper::checkTime(unsigned eid, double dt,
                                      const std::vector<std::string>& params,
                                      Properties& properties)
{
    assert(params.size() == 1); //params=[timer]
    if (!lib::hasKey(properties, "timer"))
        properties["timer"] = std::stod(params[0]);
    if (properties["timer"].asDouble() <= 0)
    {
        //timeout
        properties.erase("timer");
        return state::FAILURE;
    }
    properties["timer"] = properties["timer"].asDouble() - dt;
    return state::SUCCESS;
}

behaviour::nState AIHelper::checkCollision(unsigned eid, const std::vector<std::string>& params)
{
    assert(params.size() == 0); //no params
    if (!ecs::has<cp::Physics>(eid) ||
        ecs::get<cp::Physics>(eid).collisionState != PhysicsComponent::CollisionType::NONE)
        return state::FAILURE;
    return state::SUCCESS;
}

behaviour::nState AIHelper::execAnim(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    if (!ecs::has<cp::Render>(eid))
        return state::FAILURE;
    
    if (!lib::hasKey(properties, "done"))
    {
        properties["done"] = false;
        assert(params.size() > 0); // params=[keyName, repeat]
        std::string keyName = params[0];
        int repeat = -1;
        if (params.size() > 1)
            repeat = std::stoi(params[1]);
        ecs::get<cp::Render>(eid).setAnimation(keyName, repeat, [&properties](bool){
            properties["done"] = true;
        });
    }
    return properties["done"].asBool() ? state::SUCCESS : state::RUNNING;
}

behaviour::nState AIHelper::execTargetMood(unsigned eid, float sight,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 2); //params=[category,value]
    
    auto moodGroup = getMoodGroup(ecs::get<cp::Mood>(eid), params[1]);
    auto targetID = SysHelper::getNearest(system->context->ecs->getID(), eid, moodGroup, sight);
    if (targetID != nullptr)
    {
        system->context->ecs->add<cp::Target>(eid) = targetID.Value;
        return state::SUCCESS;
    }
    return state::FAILURE;
}

behaviour::nState AIHelper::execTargetNone(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    system->context->ecs->del<cp::Target>(eid);
    return state::SUCCESS;

}

behaviour::nState AIHelper::execMoveToRand(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    
    if (!ecs::has<cp::Input>(eid))
        return state::FAILURE;
    
    if (!lib::hasKey(properties, "target"))
    {
        auto grid = system->context->data->getGrid();
        lib::v2u pos = {
            system->random->interval((unsigned)0, grid.width),
            system->random->interval((unsigned)0, grid.height - 1)
        };
        
        auto maxCount = 1000;
        while(maxCount-- > 0 &&
              (!lib::hasKey(grid.get({pos.x, pos.y}).fields, BlockInfo::collision) ||
               grid.get({pos.x, pos.y}).fields[BlockInfo::collision] != "walkable"))
        {
            pos = {
                system->random->interval((unsigned)0, grid.width),
                system->random->interval((unsigned)0, grid.height - 1)
            };
        }
        
        properties["target"] = cc::ValueMap{{"x", cc::Value((int)pos.x)},
            {"y", cc::Value((int)pos.y)}};
    }
    
    auto& cpInput = ecs::get<cp::Input>(eid);
    auto bounds = SysHelper::getBounds(eid);
    auto bounds2 = system->context->data->getBlockBound({
        (unsigned)properties["target"].asValueMap()["x"].asInt(),
        (unsigned)properties["target"].asValueMap()["y"].asInt()
    });
    auto vdir = cc::Vec2(bounds2.getMidX() - bounds.getMidX(), bounds2.getMidY() - bounds.getMidY());
    if (vdir.length() < 4)
    {
        cpInput.direction = cc::Vec2::ZERO;
        return state::SUCCESS;
    }
    cpInput.direction = vdir;
    return state::RUNNING;
}

behaviour::nState AIHelper::execMoveStop(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    if (!ecs::has<cp::Input>(eid))
        ecs::get<cp::Input>(eid).direction = cc::Vec2::ZERO;
    return state::SUCCESS;
}

behaviour::nState AIHelper::execMoveToSleepZone(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    //dirty cast
    RoomData* roomData = dynamic_cast<RoomData*>(system->context->data);
    if (roomData)
    {
        if (!ecs::has<cp::Input>(eid))
            return state::FAILURE;
        if (!lib::hasKey(properties, "target"))
        {
            if (!ecs::has<cp::AI, cp::Position, cp::Physics>(eid))
                return state::FAILURE;
            auto bounds = SysHelper::getBounds(eid);
            auto sleepZone = roomData->getSleepZone(ecs::get<cp::AI>(eid).sleep,
                                                    {bounds.getMidX(), bounds.getMidY()});
            if (sleepZone == nullptr)
                return state::FAILURE;
            sleepZone->taken = true;
            auto& shape = ecs::get<cp::Physics>(eid).shape;
            cc::Point pos = {
                (sleepZone->bounds.getMidX() - bounds.size.width / 2) - shape.origin.x,
                (sleepZone->bounds.getMidY() - bounds.size.height / 2) - shape.origin.y
            };
            properties["target"] = cc::ValueMap{{"x", cc::Value((int)pos.x)},
                                                {"y", cc::Value((int)pos.y)}};
        }
        
        auto& cpInput = ecs::get<cp::Input>(eid);
        auto currentPos = ecs::get<cp::Position>(eid).pos;
        auto vdir = cc::Vec2(properties["target"].asValueMap()["x"].asInt() - currentPos.x,
                             properties["target"].asValueMap()["y"].asInt() - currentPos.y);
        if (vdir.length() < 5.0)
        {
            cpInput.direction = cc::Vec2::ZERO;
            return state::SUCCESS;
        }
        cpInput.direction = vdir;
        return state::RUNNING;
    }
    return state::FAILURE;
}

behaviour::nState AIHelper::execMoveDirRand(unsigned eid,
        const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    if (!ecs::has<cp::Input>(eid))
        return state::FAILURE;
    auto& cpInput = ecs::get<cp::Input>(eid);
    if (cpInput.direction == cc::Vec2::ZERO)
        cpInput.direction = Dir::rand().toVec();
    return state::SUCCESS;
}

behaviour::nState AIHelper::execMoveNearTarget(unsigned eid,
        const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 2); //params=[category, range]
    if (!ecs::has<cp::Target, cp::Input>(eid))
        return state::FAILURE;
    auto tid = ecs::get<cp::Target>(eid);
    if (!ecs::has<cp::Position, cp::Physics>(tid))
        return state::FAILURE;
    
    auto& cpInput = ecs::get<cp::Input>(eid);
    auto bounds = SysHelper::getBounds(eid);
    auto bounds2 = SysHelper::getBounds(tid);
    auto vdir = cc::Vec2(bounds2.getMidX() - bounds.getMidX(), bounds2.getMidY() - bounds.getMidY());
    if (vdir.length() < std::stod(params[1]))
    {
        cpInput.direction = cc::Vec2::ZERO;
        return state::SUCCESS;
    }
    cpInput.direction = vdir;
    return state::RUNNING;
}

behaviour::nState AIHelper::execStopSleepZone(unsigned eid,
        const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    auto& cpAI = ecs::get<cp::AI>(eid);
    RoomData* roomData = dynamic_cast<RoomData*>(system->context->data);
    if (roomData)
    {
        if (!ecs::has<cp::Position, cp::Physics>(eid))
            return state::FAILURE;
        auto bounds = SysHelper::getBounds(eid);
        roomData->freeSleepZone(cpAI.sleep,
                                {bounds.getMidX(), bounds.getMidY()});
    }
    return state::SUCCESS;
}