#include "Headers.h"


void AISystem::tick(double dt)
{
    using namespace std::placeholders;
    
    for(auto eid : ecs.join<cp::Input, cp::AI>())
    {
        if (ecs::get<cp::Input>(eid).disabled)
            continue; //skip disabled entities
        
        auto& cpAI = ecs::get<cp::AI>(eid);

        cpAI.board.onCheck = [eid, this](unsigned nid) {
            return this->onCheck(eid, nid);
        };
        cpAI.board.onExecute = [eid, this](unsigned nid) {
            return this->onExecute(eid, nid);
        };
        
        auto status = cpAI.getNode2Run()->visit(cpAI.board);
        CCASSERT(status == behaviour::nState::RUNNING, "corrupted behaviours loop");
        
        /*auto& cpInput = ecs::get<cp::Input>(eid);
        auto& cpAI = ecs::get<cp::AI>(eid);
        
        cpInput.lastOrientation = cpInput.orientation;
        if (cpAI.timer == 0)
        {
            cpAI.timer = 1.0;
            if (cpInput.orientation == Dir::None)
                cpInput.orientation = Dir::rand();
            else
                cpInput.orientation.turnLeft();
        }
        else
        {
            cpAI.timer = MAX(cpAI.timer - dt, 0);
        }*/
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
            auto properties = cpAI.board.getFields(nid);
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
        break;
        case CheckBType::IN_SIGHT: {
            assert(node->values.size() == 2); //params=[category,value]
            
        }
        break;
        default:
            Log("unsupported check: %s", node->name.c_str());
            break;
    }
    
    return state::FAILURE;
}

behaviour::nState AISystem::onExecute(unsigned eid, unsigned nid)
{
    auto& cpAI = ecs::get<cp::AI>(eid);
    behaviour::BaseNode* node = cpAI.bref->getNode(nid);

    switch(this->execMap[node->name])
    {
        case ExecBType::TARGET: {
            
        }
        break;
        case ExecBType::MOVE_DIR: {
            
        }
        break;
        case ExecBType::MOVE_NEAR: {
            
        }
        break;
        default:
            Log("unsupported action: %s", node->name.c_str());
            break;
    }

    return behaviour::nState::FAILURE;
}
