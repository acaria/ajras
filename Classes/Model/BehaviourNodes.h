#pragma once
#include "Misc.h"

namespace behaviour
{
    enum nState { RUNNING, SUCCESS, FAILURE };
    
    struct BaseNode;
    
    struct BoardNode
    {
        BoardNode()
        {
            this->onCheck = nullptr;
            this->onExecute = nullptr;
        }
        
        void reset()
        {
            states.clear();
            fields.clear();
            lasts.clear();
        }
        
        void clear(unsigned id)
        {
            states.erase(id);
            lasts.erase(id);
            fields[id].clear();
        }
        
        std::map<std::string, cc::Value>& getFields(unsigned id)
        {
            if (fields.find(id) == fields.end())
                fields[id] = std::map<std::string, cc::Value>();
            return fields[id];
        }
        
        std::map<unsigned, nState> states;
        std::map<unsigned, nState> lasts;
        
        std::function<nState(unsigned id)> onCheck;
        std::function<nState(unsigned id)> onExecute;
        
    private:
        std::map<unsigned, std::map<std::string, cc::Value>> fields;
    };

    struct BaseNode
    {
        BaseNode(unsigned id, BaseNode* parent)
        {
            this->id = id;
            this->parent = parent;
        }
    
        virtual ~BaseNode()
        {
            for(auto node : children)
            {
                if (node != nullptr)
                    delete node;
            }
        }
    
        virtual nState visit(BoardNode& board) = 0;
    
        unsigned id;
        std::string name = "";
        std::vector<std::string> values;
        std::vector<BaseNode*> children;
        BaseNode* parent;
        
        protected:
        virtual std::string toStr()
        {
            std::stringstream ss;
            ss << this->name;
            if (this->values.size() > 0)
            {
                ss << "(";
                int i = 0;
                for(auto v : this->values)
                {
                    ss << v;
                    if (i++ < this->values.size() - 1)
                        ss << ", ";
                }
                ss << ")";
            }
            return ss.str();
        }
        
        void clearNexts(BoardNode& board)
        {
            if (parent != nullptr)
            {
                std::function<void(BaseNode* n, BoardNode& b)> visitAndClear;
                visitAndClear = [&](BaseNode* n, BoardNode& b){
                    b.clear(n->id);
                    for(auto subNode : n->children)
                        visitAndClear(subNode, b);
                };
            
                bool clearMode = false;
                for(int i = 0; i < parent->children.size(); i++)
                {
                    if (parent->children[i] == this)
                        clearMode = true;
                    
                    if (clearMode)
                        visitAndClear(parent->children[i], board);
                }
            }
        }
    };
    
    struct RepeatNode : public BaseNode
    {
        RepeatNode(unsigned id, BaseNode* parent) : BaseNode(id, parent) {}
        
        nState visit(BoardNode& board)
        {
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
            {
                if (board.states[id] == SUCCESS)
                {
                    this->recursiveClearStates(this, board);
                }
                else //(board.states[id] == FAILURE)
                {
                    board.states[id] = SUCCESS;
                    return SUCCESS;
                }
            }
            board.states[id] = this->children[0]->visit(board);
            return RUNNING;
        }
        
    private:
        void recursiveClearStates(BaseNode* node, BoardNode& board)
        {
            board.clear(node->id);
            for(auto child : node->children)
                recursiveClearStates(child, board);
        }
    };

    struct SequenceNode : public BaseNode
    {
        SequenceNode(unsigned id, BaseNode* parent) : BaseNode(id, parent) {}
    
        nState visit(BoardNode& board)
        {
#if kTraceBehaviours
            Log("SequenceNode: %s", this->toStr().c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
            
            board.states[id] = RUNNING;
            for(auto node : children)
            {
                auto state = node->visit(board);
            
                if (state == FAILURE)
                {
                    board.states[id] = FAILURE;
                    return FAILURE;
                }
            
                if (state == RUNNING)
                    return RUNNING;
            }
            board.states[id] = SUCCESS;
            return SUCCESS;
        }
    };

    struct SelectorNode : public BaseNode
    {
        SelectorNode(unsigned id, BaseNode* parent) : BaseNode(id, parent) {}
    
        nState visit(BoardNode& board)
        {
#if kTraceBehaviours
            Log("SelectorNode: %s", this->toStr().c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
            
            board.states[id] = RUNNING;
            for(auto node : children)
            {
                auto state = node->visit(board);
            
                if (state == SUCCESS)
                {
                    board.states[id] = SUCCESS;
                    return SUCCESS;
                }
            
                if (state == RUNNING)
                    return RUNNING;
            }
            board.states[id] = FAILURE;
            return FAILURE;
        }
    };

    struct UntilNode : public BaseNode
    {
        UntilNode(unsigned id, BaseNode* parent) : BaseNode(id, parent) {}
        
        nState visit(BoardNode& board)
        {
#if kTraceBehaviours
            Log("UntilNode: %s", this->toStr().c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
            board.states[this->id] = RUNNING;
            
            auto untilState = board.onCheck(this->id);
            if (untilState == SUCCESS)
            {
                auto actionState = this->children[0]->visit(board);
                if (actionState != RUNNING)
                {
                    board.states[this->id] = actionState;
                    return actionState;
                }
                return RUNNING;
            }
            else //FAILURE
            {
                board.states[this->id] = FAILURE;
                return FAILURE;
            }
        }
    };
    
    struct WaitNode : public BaseNode
    {
        WaitNode(unsigned id, BaseNode* parent) : BaseNode(id, parent) {}
        
        nState visit(BoardNode& board)
        {
#if kTraceBehaviours
            Log("WaitNode: %s", this->toStr().c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
            board.states[this->id] = RUNNING;
            
            auto &properties = board.getFields(this->id);
            if (!lib::hasKey(properties, "timer"))
                properties["timer"] = lib::now();
            if (lib::now() - properties["timer"].asDouble() > std::stod(this->name))
            {
                board.states[this->id] = SUCCESS;
                properties.erase("timer");
                return SUCCESS;
            }
            return RUNNING;
        }
    };

    struct CheckNode : public BaseNode
    {
        CheckNode(unsigned id, BaseNode* parent, bool redo = false) : BaseNode(id, parent),
                                                                      redo(redo) {}
    
        nState visit(BoardNode& board)
        {
#if kTraceBehaviours
            Log("CheckNode: %s", this->toStr().c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
        
            auto state = board.onCheck(this->id);
            if (!redo)
                board.states[this->id] = state;
            return state;
        }
        
    private:
        bool redo;
    };

    struct ActionNode : public BaseNode
    {
        ActionNode(unsigned id, BaseNode* parent, bool redo = false) : BaseNode(id, parent),
                                                                       redo(redo) {}
    
        nState visit(BoardNode& board)
        {
#if kTraceBehaviours
            Log("ActionNode: %s", this->toStr().c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
        
            auto state = board.onExecute(this->id);
            if (!redo)
                board.states[this->id] = state;
            else
            {
                if (!lib::hasKey(board.lasts, this->id))
                    board.lasts[this->id] = state;
                else if (board.lasts[this->id] != state)
                {
                    this->clearNexts(board);
                    board.lasts[this->id] = state;
                }
            }
            return state;
        }
        
    private:
        bool redo;
    };
    
    static std::map<std::string, std::function<BaseNode*(unsigned id, BaseNode* parent)>> factory {
        {"selector", [](unsigned id, BaseNode* parent) { return new SelectorNode(id, parent); }},
        {"sequence", [](unsigned id, BaseNode* parent) { return new SequenceNode(id, parent); }},
        {"repeat", [](unsigned id, BaseNode* parent) { return new RepeatNode(id, parent); }},
        {"wait", [](unsigned id, BaseNode* parent) { return new WaitNode(id, parent); }},
        {"until", [](unsigned id, BaseNode* parent) { return new UntilNode(id, parent); }},
        {"check", [](unsigned id, BaseNode* parent) { return new CheckNode(id, parent); }},
        {"check+", [](unsigned id, BaseNode* parent) { return new CheckNode(id, parent, true); }},
        {"action", [](unsigned id, BaseNode* parent) { return new ActionNode(id, parent); }},
        {"action+", [](unsigned id, BaseNode* parent) { return new ActionNode(id, parent, true); }},
    };
}
