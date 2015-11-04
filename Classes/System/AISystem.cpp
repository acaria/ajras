#include "AISystem.h"
#include "ECSGroup.h"
#include "Components.h"
#include "SysHelper.h"
#include "BehaviourData.h"
#include "BlockInfo.h"
#include "RoomData.h"

void AISystem::init(IMapData *data)
{
    this->data = data;
}

void AISystem::tick(double dt)
{
    using namespace std::placeholders;
    
    for(auto eid : ecs.join<cp::Input, cp::AI, cp::Mood>())
    {
        if (ecs::has<cp::Control>(eid))
            continue; //manual
        if (ecs::get<cp::Input>(eid).disabled)
            continue; //skip disabled entities
        
        auto& cpAI = ecs::get<cp::AI>(eid);

        cpAI.board.onCheck = [eid, this](unsigned nid) {
            return this->onCheck(eid, nid);
        };
        cpAI.board.onExecute = [eid, this](unsigned nid) {
            return this->onExecute(eid, nid);
        };
        
        auto status = cpAI.bref->getRootNode()->visit(cpAI.board);
        if (status != behaviour::nState::RUNNING)
            cpAI.reset();
        //CCASSERT(status == behaviour::nState::RUNNING, "corrupted behaviours loop");
    }
}

behaviour::nState AISystem::onCheck(unsigned eid, unsigned nid)
{
    using state = behaviour::nState;
    auto& cpAI = ecs::get<cp::AI>(eid);
    auto mood = ecs::get<cp::Mood>(eid);
    behaviour::BaseNode* node = cpAI.bref->getNode(nid);
    
    switch(this->checkMap[node->name])
    {
        case CheckBType::NEAR: {
            assert(node->values.size() > 0);
            switch(AIComponent::mapType[node->values[0]])
            {
                case AIComponent::eType::MOOD: {
                    assert(node->values.size() == 2); //params=[category,value]
                    auto maxDist = cpAI.sightRange * cpAI.sightRange;
                    auto moodGroup = this->getMoodGroup(mood, node->values[1]);
                    auto bounds = SysHelper::getBounds(eid);
                    
                    float nearest = maxDist;
                    unsigned targetID = 0;
                    for(auto tid : ecs.join<cp::AI, cp::Position, cp::Collision, cp::Mood>())
                    {
                        if (tid == eid)
                            continue;
                        if (!def::mood::inside(ecs::get<cp::Mood>(tid), moodGroup))
                            continue;
                        auto bounds2 = SysHelper::getBounds(tid);
                        float dist = cc::Vec2(
                            bounds.getMidX() - bounds2.getMidX(),
                            bounds.getMidY() - bounds2.getMidY()).lengthSquared();
                        if (dist < maxDist && dist < nearest)
                        {
                            nearest = dist;
                            targetID = tid;
                        }
                    }
                    return (targetID != 0) ? state::SUCCESS : state::FAILURE;
                }
                default: {
                    Log("unrecognised category type : %s", node->values[0].c_str());
                    break;
                }
            }
        }
        case CheckBType::TIME: {
            assert(node->values.size() == 1); //params=[timer]
            auto &properties = cpAI.board.getFields(nid);
            if (!lib::hasKey(properties, "timer"))
                properties["timer"] = lib::now();
            if (lib::now() - properties["timer"].asDouble() > std::stod(node->values[0]))
            {
                //timeout
                properties.erase("timer");
                return state::FAILURE;
            }
            return state::SUCCESS;
        }
        case CheckBType::COLLISION:
            assert(node->values.size() == 0); //no params
            if (!ecs::has<cp::Collision>(eid) ||
                ecs::get<cp::Collision>(eid).current != CollisionComponent::CType::NONE)
                return state::FAILURE;
            return state::SUCCESS;
        default:
            Log("unsupported check: %s", node->name.c_str());
            break;
    }
    
    return state::FAILURE;
}

def::mood::Flags AISystem::getMoodGroup(def::mood::Flags ref,
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

behaviour::nState AISystem::onExecute(unsigned eid, unsigned nid)
{
    using state = behaviour::nState;
    auto& cpAI = ecs::get<cp::AI>(eid);
    auto mood = ecs::get<cp::Mood>(eid);
    behaviour::BaseNode* node = cpAI.bref->getNode(nid);

    switch(this->execMap[node->name])
    {
        case ExecBType::ANIM: {
            if (!ecs::has<cp::Render>(eid))
                return state::FAILURE;
            auto &properties = cpAI.board.getFields(nid);
            if (!lib::hasKey(properties, "done"))
            {
                properties["done"] = false;
                assert(node->values.size() > 0); // params=[keyName, repeat]
                std::string keyName = node->values[0];
                int repeat = -1;
                if (node->values.size() > 1)
                    repeat = std::stoi(node->values[1]);
                ecs::get<cp::Render>(eid).setAnimation(keyName, repeat, [eid, nid](bool){
                    auto& cpAI = ecs::get<cp::AI>(eid);
                    cpAI.board.getFields(nid)["done"] = true;
                });
            }
            return properties["done"].asBool() ? state::SUCCESS : state::RUNNING;
        }
        case ExecBType::TARGET: {
            assert(node->values.size() > 0);
            switch(AIComponent::mapType[node->values[0]])
            {
                case AIComponent::eType::MOOD: {
                    assert(node->values.size() == 2); //params=[category,value]
                    
                    auto moodGroup = this->getMoodGroup(mood, node->values[1]);
                    auto targetID = SysHelper::getNearest(ecs.getID(), eid, moodGroup, cpAI.sightRange);
                    if (targetID != 0)
                    {
                        ecs.add<cp::Target>(eid) = targetID;
                        return state::SUCCESS;
                    }
                    return state::FAILURE;
                }
                case AIComponent::eType::NONE: {
                    assert(node->values.size() == 1); //params=[category]
                    ecs.del<cp::Target>(eid);
                    return state::SUCCESS;
                }
                default: {
                    Log("unrecognised category type : %s", node->values[0].c_str());
                    break;
                }
            }
        }
        break;
        case ExecBType::MOVE_TO: {
            assert(node->values.size() == 1); //params=[type]
            
            auto& cpInput = ecs::get<cp::Input>(eid);
            switch(actionMap[node->values[0]])
            {
                case ActionBType::RAND: {
                    if (!ecs::has<cp::Input>(eid))
                        return state::FAILURE;
                
                    auto &properties = cpAI.board.getFields(nid);
                    if (!lib::hasKey(properties, "target"))
                    {
                        auto grid = this->data->getGrid();
                        lib::v2u pos = {
                            random.interval((unsigned)0, grid.width),
                            random.interval((unsigned)0, grid.height - 1)
                        };
                        
                        auto maxCount = 1000;
                        while(maxCount-- > 0 &&
                              (!lib::hasKey(grid.get({pos.x, pos.y}).fields, BlockInfo::collision) ||
                              grid.get({pos.x, pos.y}).fields[BlockInfo::collision] != "walkable"))
                        {
                            pos = {
                                random.interval((unsigned)0, grid.width),
                                random.interval((unsigned)0, grid.height - 1)
                            };
                        }
                        
                        properties["target"] = cc::ValueMap{{"x", cc::Value((int)pos.x)},
                                                            {"y", cc::Value((int)pos.y)}};
                    }
                    
                    auto& cpInput = ecs::get<cp::Input>(eid);
                    auto bounds = SysHelper::getBounds(eid);
                    auto bounds2 = data->getBlockBound({
                        (unsigned)properties["target"].asValueMap()["x"].asInt(),
                        (unsigned)properties["target"].asValueMap()["y"].asInt()
                    });
                    auto vdir = cc::Vec2(bounds2.getMidX() - bounds.getMidX(), bounds2.getMidY() - bounds.getMidY());
                    if (vdir.length() < 4)
                    {
                        cpInput.setDirection(Dir::None);
                        return state::SUCCESS;
                    }
                    cpInput.setDirection(vdir.getNormalized());
                    return state::RUNNING;
                }
                case ActionBType::STOP: {
                    cpInput.setDirection(Dir::None);
                    return state::SUCCESS;
                }
                case ActionBType::SLEEPZONE: {
                    RoomData* roomData = dynamic_cast<RoomData*>(data);
                    if (roomData)
                    {
                        if (!ecs::has<cp::Input>(eid))
                            return state::FAILURE;
                        auto &properties = cpAI.board.getFields(nid);
                        if (!lib::hasKey(properties, "target"))
                        {
                            if (!ecs::has<cp::AI, cp::Position, cp::Collision>(eid))
                                return state::FAILURE;
                            auto bounds = SysHelper::getBounds(eid);
                            auto sleepZone = roomData->getSleepZone(ecs::get<cp::AI>(eid).sleep, {bounds.getMidX(), bounds.getMidY()});
                            if (sleepZone == nullptr)
                                return state::FAILURE;
                            sleepZone->taken = true;
                            cc::Point pos = {
                                (sleepZone->bounds.getMidX() - bounds.size.width / 2) - ecs::get<cp::Collision>(eid).rect.origin.x,
                                (sleepZone->bounds.getMidY() - bounds.size.height / 2) - ecs::get<cp::Collision>(eid).rect.origin.y
                            };
                            properties["target"] = cc::ValueMap{{"x", cc::Value((int)pos.x)}, {"y", cc::Value((int)pos.y)}};
                        }
                    
                        auto& cpInput = ecs::get<cp::Input>(eid);
                        auto currentPos = ecs::get<cp::Position>(eid).pos;
                        auto vdir = cc::Vec2(properties["target"].asValueMap()["x"].asInt() - currentPos.x,
                                             properties["target"].asValueMap()["y"].asInt() - currentPos.y);
                        if (vdir.length() < 5.0)
                        {
                            cpInput.setDirection(Dir::None);
                            return state::SUCCESS;
                        }
                        cpInput.setDirection(vdir);
                        return state::RUNNING;
                    }
                }
                default:
                    Log("invalid sub parameter: %s", node->values[0].c_str());
                    break;
            }
        }
        case ExecBType::MOVE_DIR: {
            assert(node->values.size() == 1); //params=[type]
            
            auto& cpInput = ecs::get<cp::Input>(eid);
            switch(actionMap[node->values[0]])
            {
                case ActionBType::RAND: {
                    if (cpInput.orientation == Dir::None)
                        cpInput.setDirection(Dir::rand());
                    return state::RUNNING;
                }
                case ActionBType::STOP: {
                    cpInput.setDirection(Dir::None);
                    return state::SUCCESS;
                }
                default:
                    Log("invalid sub parameter: %s", node->values[0].c_str());
                    break;
            }
        }
        case ExecBType::MOVE_NEAR: {
            assert(node->values.size() == 2); //params=[type, range]
            switch(actionMap[node->values[0]])
            {
                case ActionBType::TARGET: {
                    if (!ecs::has<cp::Target, cp::Input>(eid))
                        return state::FAILURE;
                    auto tid = ecs::get<cp::Target>(eid);
                    if (!ecs::has<cp::Position, cp::Collision>(tid))
                        return state::FAILURE;
                    
                    auto& cpInput = ecs::get<cp::Input>(eid);
                    auto bounds = SysHelper::getBounds(eid);
                    auto bounds2 = SysHelper::getBounds(tid);
                    auto vdir = cc::Vec2(bounds2.getMidX() - bounds.getMidX(), bounds2.getMidY() - bounds.getMidY());
                    if (vdir.length() < std::stod(node->values[1]))
                    {
                        cpInput.setDirection(Dir::None);
                        return state::SUCCESS;
                    }
                    cpInput.setDirection(vdir);
                    return state::RUNNING;
                }
                default:
                    Log("invalid move near sub parameter: %s", node->values[0].c_str());
                    break;
            }
        }
        case ExecBType::CHARGE: {
            assert(node->values.size() == 3); //params=[type, duration_load, duration_charge]
            switch(actionMap[node->values[0]])
            {
                case ActionBType::TARGET: {
                    if (!ecs::has<cp::Target, cp::Render, cp::Input, cp::Melee, cp::Collision>(eid))
                        return state::FAILURE;
                    
                    auto &properties = cpAI.board.getFields(nid);
                    
                    if (!lib::hasKey(properties, "target"))
                    {
                        auto tid = ecs::get<cp::Target>(eid);
                        if (!ecs::has<cp::Position, cp::Collision>(tid))
                            return state::FAILURE;
                        
                        auto bounds = SysHelper::getBounds(eid);
                        auto bounds2 = SysHelper::getBounds(tid);
                        auto vdir = cc::Vec2(bounds2.getMidX() - bounds.getMidX(),
                                             bounds2.getMidY() - bounds.getMidY());
                        properties["target"] = cc::ValueMap{{"x", cc::Value((float)vdir.x)},
                                                            {"y", cc::Value((float)vdir.y)}};
                        properties["time_load"] = lib::now() + std::stod(node->values[1]);
                        properties["time_charge"] = properties["time_load"].asDouble() + std::stod(node->values[2]);
                        properties["save_ratio"] = ecs::has<cp::Velocity>(eid) ? ecs::get<cp::Velocity>(eid).ratio : 1.0;
                        ecs::get<cp::Render>(eid).setAnimation("charge_load", -1);
                    }
                    
                    auto now = lib::now();
                    
                    if (now > properties["time_charge"].asDouble()/* ||
                        ecs::get<cp::Collision>(eid).current == CollisionComponent::CType::DECOR*/)
                    {
                        if (ecs::has<cp::Velocity>(eid))
                            ecs::get<cp::Velocity>(eid).ratio = properties["save_ratio"].asFloat();
                        //ecs::get<cp::Input>(eid).actionMode = ActionMode::explore;
                        ecs::get<cp::Render>(eid).setAnimation("idle", -1);
                        ecs::get<cp::Melee>(eid).processing = true;
                        return state::SUCCESS;
                    }
                    if (now > properties["time_load"].asDouble())
                    {
                        if (!lib::hasKey(properties, "charging"))
                        {
                            properties["charging"] = true;
                            if (ecs::has<cp::Velocity>(eid))
                                ecs::get<cp::Velocity>(eid).ratio = 3.0;
                            ecs::get<cp::Render>(eid).setAnimation("charge_atk", -1);
                            //ecs::get<cp::Input>(eid).actionMode = ActionMode::attack;
                        }
                        else
                        {
                            cc::Vec2 vdir = {
                                properties["target"].asValueMap()["x"].asFloat(),
                                properties["target"].asValueMap()["y"].asFloat()
                            };
                            ecs::get<cp::Input>(eid).setDirection(vdir);
                        }
                    }
                    return state::RUNNING;
                }
                default:
                    Log("invalid charge sub parameter: %s", node->values[0].c_str());
                    break;
            }
        }
        case ExecBType::STOP: {
            assert(node->values.size() == 1); //params=[type]
            switch(actionMap[node->values[0]])
            {
                case ActionBType::SLEEPZONE: {
                    RoomData* roomData = dynamic_cast<RoomData*>(this->data);
                    if (roomData)
                    {
                        if (!ecs::has<cp::Position, cp::Collision>(eid))
                            return state::FAILURE;
                        auto bounds = SysHelper::getBounds(eid);
                        roomData->freeSleepZone(cpAI.sleep,
                            {bounds.getMidX(), bounds.getMidY()});
                    }
                    return state::SUCCESS;
                }
                default:
                    Log("invalid stop sub parameter: %s", node->values[0].c_str());
                    break;
            }
        }
        default:
            Log("unsupported action: %s", node->name.c_str());
            break;
    }

    return state::FAILURE;
}
