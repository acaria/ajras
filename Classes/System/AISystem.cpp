#include "AISystem.h"
#include "ECSGroup.h"
#include "Components.h"
#include "BehaviourData.h"
#include "BlockInfo.h"
#include "RoomData.h"
#include "CmdFactory.h"
#include "AIHelper.h"

void AISystem::tick(double dt)
{
    using namespace std::placeholders;
    
    for(auto eid : context->ecs->join<cp::Input, cp::AI, cp::Mood, cp::Position>())
    {
        if (ecs::has<cp::Control>(eid))
            continue; //manual
        if (!ecs::get<cp::Input>(eid).enabled)
        {
            ecs::get<cp::AI>(eid).reset();
            continue; //skip disabled entities
        }
        auto& cpAI = ecs::get<cp::AI>(eid);
        cpAI.processing = true;

        cpAI.board.onCheck = [eid, this, dt](unsigned nid) {
            return this->onCheck(eid, nid, dt);
        };
        cpAI.board.onExecute = [eid, this, dt](unsigned nid) {
            return this->onExecute(eid, nid, dt);
        };
        
        auto status = cpAI.bref->getRootNode()->visit(cpAI.board, dt);
        if (status != behaviour::nState::RUNNING)
            cpAI.reset();
    }
}

behaviour::nState AISystem::onCheck(unsigned eid, unsigned nid, double dt)
{
    using state = behaviour::nState;
    auto& cpAI = ecs::get<cp::AI>(eid);
    behaviour::BaseNode* node = cpAI.bref->getNode(nid);
    auto& properties = cpAI.board.getFields(nid);
    
    switch(lib::hash(node->name))
    {
        case lib::hash("near"):
        {
            assert(node->values.size() > 0);
            switch(lib::hash(node->values[0]))
            {
                case lib::hash("mood"):
                    return cmd.checkNearMood(eid, cpAI.sightRange.first, node->values, properties);
                default:
                    Log("unrecognised category type : %s", node->values[0].c_str());
                    break;
            }
        }
        case lib::hash("time"):
            return cmd.checkTime(eid, dt, node->values, properties);
        case lib::hash("collision"):
            return cmd.checkCollision(eid, node->values);
        default:
        {
            Log("unsupported check: %s", node->name.c_str());
            break;
        }
    }
    
    return state::FAILURE;
}

behaviour::nState AISystem::onExecute(unsigned eid, unsigned nid, double dt)
{
    using state = behaviour::nState;
    auto& cpAI = ecs::get<cp::AI>(eid);
    behaviour::BaseNode* node = cpAI.bref->getNode(nid);
    auto &properties = cpAI.board.getFields(nid);

    switch(lib::hash(node->name))
    {
        case lib::hash("anim"):
            return cmd.execAnim(eid, node->values, properties);
        case lib::hash("target"):
        {
            assert(node->values.size() > 0);
            switch(lib::hash(node->values[0]))
            {
                case lib::hash("mood"):
                    return cmd.execTargetMood(eid, cpAI.sightRange.second,
                                              node->values, properties);
                case lib::hash("none"):
                    return cmd.execTargetNone(eid, node->values, properties);
                default:
                {
                    Log("unrecognised category type : %s", node->values[0].c_str());
                    break;
                }
            }
        }
        break;
        case lib::hash("moveto"):
        {
            assert(node->values.size() > 0); //params=[type]
            switch(lib::hash(node->values[0]))
            {
                case lib::hash("rand"):
                    return cmd.execMoveToRand(eid, node->values, properties);
                case lib::hash("stop"):
                    return cmd.execMoveStop(eid, node->values, properties);
                case lib::hash("sleep_zone"):
                    return cmd.execMoveToSleepZone(eid, node->values, properties);
                default:
                {
                    Log("invalid sub parameter: %s", node->values[0].c_str());
                    return state::FAILURE;
                }
            }
        }
        case lib::hash("movedir"):
        {
            assert(node->values.size() > 0); //params=[type]
            switch(lib::hash(node->values[0]))
            {
                case lib::hash("rand"):
                    return cmd.execMoveDirRand(eid, node->values, properties);
                case lib::hash("stop"):
                    return cmd.execMoveStop(eid, node->values, properties);
                default:
                {
                    Log("invalid sub parameter: %s", node->values[0].c_str());
                    return state::FAILURE;
                }
            }
        }
        case lib::hash("movenear"):
        {
            assert(node->values.size() > 0); //params=[type, range]
            switch(lib::hash(node->values[0]))
            {
                case lib::hash("target"):
                    return cmd.execMoveNearTarget(eid, node->values, properties);
                default:
                {
                    Log("invalid move near sub parameter: %s", node->values[0].c_str());
                    return state::FAILURE;
                }
            }
        }
        case lib::hash("charge"):
        {
            /*
            assert(node->values.size() == 3); //params=[type, duration_load, duration_charge]
            switch(actionMap[node->values[0]])
            {
                case ActionBType::TARGET: {
                    if (!ecs::has<cp::Target, cp::Render, cp::Input, cp::Melee, cp::Physics>(eid))
                        return state::FAILURE;
                    
                    auto &properties = cpAI.board.getFields(nid);
                    
                    if (!lib::hasKey(properties, "target"))
                    {
                        auto tid = ecs::get<cp::Target>(eid);
                        if (!ecs::has<cp::Position, cp::Physics>(tid))
                            return state::FAILURE;
                        
                        auto bounds = SysHelper::getBounds(eid);
                        auto bounds2 = SysHelper::getBounds(tid);
                        auto vdir = cc::Vec2(bounds2.getMidX() - bounds.getMidX(),
                                             bounds2.getMidY() - bounds.getMidY());
                        properties["target"] = cc::ValueMap{{"x", cc::Value((float)vdir.x)},
                                                            {"y", cc::Value((float)vdir.y)}};
                        properties["time_load"] = std::stod(node->values[1]);
                        properties["time_charge"] = properties["time_load"].asDouble() + std::stod(node->values[2]);
                        properties["save_ratio"] = ecs::has<cp::Physics>(eid) ? ecs::get<cp::Physics>(eid).move.ratio : 1.0;
                        ecs::get<cp::Render>(eid).setAnimation("charge_load", -1);
                    }
                    
                    if (properties["time_charge"].asDouble() <= 0)
                    {
                        if (ecs::has<cp::Physics>(eid))
                            ecs::get<cp::Physics>(eid).move.ratio = properties["save_ratio"].asFloat();
                        ecs::get<cp::Render>(eid).setAnimation("idle", -1);
                        context->ecs->del<cp::Untargetable>(eid);
                        ecs::get<cp::Melee>(eid).enabled = false;
                        return state::SUCCESS;
                    }
                    if (properties["time_load"].asDouble() <= 0)
                    {
                        if (!lib::hasKey(properties, "charging"))
                        {
                            properties["charging"] = true;
                            if (ecs::has<cp::Physics>(eid))
                                ecs::get<cp::Physics>(eid).move.ratio = 3.0;
                            context->ecs->add<cp::Untargetable>(eid) = true;
                            ecs::get<cp::Melee>(eid).enabled = true;
                            ecs::get<cp::Render>(eid).setAnimation("charge_atk", -1);
                        }
                        else
                        {
                            cc::Vec2 vdir = {
                                properties["target"].asValueMap()["x"].asFloat(),
                                properties["target"].asValueMap()["y"].asFloat()
                            };
                            ecs::get<cp::Input>(eid).direction = vdir.getNormalized();
                        }
                    }
                    properties["time_charge"] = properties["time_charge"].asDouble() - dt;
                    properties["time_load"] = properties["time_load"].asDouble() - dt;
                    return state::RUNNING;
                }
                default:
                    Log("invalid charge sub parameter: %s", node->values[0].c_str());
                    break;
            } */
            return state::FAILURE;
        }
        case lib::hash("stop"):
        {
            assert(node->values.size() == 1); //params=[type]
            switch(lib::hash(node->values[0]))
            {
                case lib::hash("sleep_zone"):
                    return cmd.execStopSleepZone(eid, node->values, properties);
                default:
                {
                    Log("invalid stop sub parameter: %s", node->values[0].c_str());
                    break;
                }
            }
        }
        default:
        {
            Log("unsupported action: %s", node->name.c_str());
            break;
        }
    }

    return state::FAILURE;
}
