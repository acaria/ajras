#include "Headers.h"

void AISystem::init(RoomData *data)
{
    this->data = data;
}

void AISystem::tick(double dt)
{
    using namespace std::placeholders;
    
    for(auto eid : ecs.join<cp::Input, cp::AI>())
    {
        if (ecs::get<cp::Input>(eid).disabled)
            continue; //skip disabled entities
        
        auto& cpAI = ecs::get<cp::AI>(eid);

#if kDrawDebug
        auto& cpRender = ecs::get<cp::Render>(eid);
        cpRender.sight->setVisible(true);
        cpRender.sight->setScale(cpAI.sightRange * 2 / kBlockSize,
                                 cpAI.sightRange * 2 / kBlockSize);
#endif

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
    behaviour::BaseNode* node = cpAI.bref->getNode(nid);
    
    switch(this->checkMap[node->name])
    {
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
            if (!ecs::has<cp::Collision>(eid) || ecs::get<cp::Collision>(eid).collide)
                return state::FAILURE;
            return state::SUCCESS;
        default:
            Log("unsupported check: %s", node->name.c_str());
            break;
    }
    
    return state::FAILURE;
}

behaviour::nState AISystem::onExecute(unsigned eid, unsigned nid)
{
    using state = behaviour::nState;
    auto& cpAI = ecs::get<cp::AI>(eid);
    behaviour::BaseNode* node = cpAI.bref->getNode(nid);

    switch(this->execMap[node->name])
    {
        case ExecBType::TARGET: {
            assert(node->values.size() > 0);
            switch(CategoryComponent::mapType[node->values[0]])
            {
                case CategoryComponent::eType::MOOD: {
                    assert(node->values.size() == 2); //params=[category,value]
                    auto maxDist = cpAI.sightRange * cpAI.sightRange;
                    auto targetMood = CategoryComponent::mapMood[node->values[1]];
                    auto bounds = SysHelper::getBounds(eid);
                    
                    float nearest = maxDist;
                    unsigned targetID = 0;
                    for(auto tid : ecs.join<cp::Cat, cp::Position, cp::Collision>())
                    {
                        if (tid == eid)
                            continue;
                        if (targetMood != ecs::get<cp::Cat>(tid).mood)
                            continue;
                        auto bounds2 = SysHelper::getBounds(tid);
                        float dist = Vec2(
                            bounds.getMidX() - bounds2.getMidX(),
                            bounds.getMidY() - bounds2.getMidY()).lengthSquared();
                        if (dist < maxDist && dist < nearest)
                        {
                            nearest = dist;
                            targetID = tid;
                        }
                    }
                    
                    if (targetID != 0)
                    {
                        ecs.add<cp::Target>(eid) = targetID;
                        return state::SUCCESS;
                    }
                    return state::FAILURE;
                }
                case CategoryComponent::eType::NONE: {
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
                        auto model = this->data->getModel();
                        lib::v2u pos = {
                            lib::randAB(0, model->grid.width), lib::randAB(0, model->grid.height)
                        };
                        
                        auto maxCount = 1000;
                        while(maxCount-- > 0 &&
                              (!lib::hasKey(model->grid.get({pos.x, pos.y}).fields, BlockInfo::collision) ||
                              model->grid.get({pos.x, pos.y}).fields[BlockInfo::collision] != "walkable"))
                        {
                            pos = {
                                lib::randAB(0, model->grid.width), lib::randAB(0, model->grid.height)
                            };
                        }
                        
                        properties["target"] = ValueMap{{"x", Value((int)pos.x)}, {"y", Value((int)pos.y)}};
                    }
                    
                    auto& cpInput = ecs::get<cp::Input>(eid);
                    auto bounds = SysHelper::getBounds(eid);
                    auto bounds2 = data->getModel()->getRectCoord({
                        (unsigned)properties["target"].asValueMap()["x"].asInt(),
                        (unsigned)properties["target"].asValueMap()["y"].asInt()
                    });
                    auto vdir = Vec2(bounds2.getMidX() - bounds.getMidX(), bounds2.getMidY() - bounds.getMidY());
                    if (vdir.length() < 10)
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
                    auto vdir = Vec2(bounds2.getMidX() - bounds.getMidX(), bounds2.getMidY() - bounds.getMidY());
                    if (vdir.length() < std::stod(node->values[1]))
                    {
                        cpInput.setDirection(Dir::None);
                        return state::SUCCESS;
                    }
                    cpInput.setDirection(vdir.getNormalized());
                    return state::RUNNING;
                }
                default:
                    Log("invalid sub parameter: %s", node->values[0].c_str());
                    break;
            }
        }
        default:
            Log("unsupported action: %s", node->name.c_str());
            break;
    }

    return state::FAILURE;
}
