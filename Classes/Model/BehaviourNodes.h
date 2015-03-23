#pragma once
#include "Headers.h"

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
        }
        
        void clear(unsigned id)
        {
            states.erase(id);
            fields[id].clear();
        }
        
        std::map<std::string, cc::Value>& getFields(unsigned id)
        {
            if (fields.find(id) == fields.end())
                fields[id] = std::map<std::string, cc::Value>();
            return fields[id];
        }
        
        std::map<unsigned, nState> states;
        
        std::function<nState(unsigned id)> onCheck;
        std::function<nState(unsigned id)> onExecute;
        
    private:
        std::map<unsigned, std::map<std::string, cc::Value>> fields;
    };

    struct BaseNode
    {
        BaseNode(unsigned id)
        {
            this->id = id;
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
    };
    
    struct RepeatNode : public BaseNode
    {
        RepeatNode(unsigned id) : BaseNode(id) {}
        
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
        SequenceNode(unsigned id) : BaseNode(id) {}
    
        nState visit(BoardNode& board)
        {
#ifdef kTraceBehaviours
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
        SelectorNode(unsigned id) : BaseNode(id) {}
    
        nState visit(BoardNode& board)
        {
#ifdef kTraceBehaviours
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
        UntilNode(unsigned id) : BaseNode(id) {}
        
        nState visit(BoardNode& board)
        {
#ifdef kTraceBehaviours
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

    struct CheckNode : public BaseNode
    {
        CheckNode(unsigned id, bool redo = false) : BaseNode(id),
                                                    redo(redo) {}
    
        nState visit(BoardNode& board)
        {
#ifdef kTraceBehaviours
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
        ActionNode(unsigned id, bool redo = false) : BaseNode(id),
                                                     redo(redo) {}
    
        nState visit(BoardNode& board)
        {
#ifdef kTraceBehaviours
            Log("ActionNode: %s", this->toStr().c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
        
            auto state = board.onExecute(this->id);
            if (!redo)
                board.states[this->id] = state;
            return state;
        }
        
    private:
        bool redo;
    };
    
    static std::map<std::string, std::function<BaseNode*(unsigned id)>> factory {
        {"selector", [](unsigned id) { return new SelectorNode(id); }},
        {"sequence", [](unsigned id) { return new SequenceNode(id); }},
        {"repeat", [](unsigned id) { return new RepeatNode(id); }},
        {"until", [](unsigned id) { return new UntilNode(id); }},
        {"check", [](unsigned id) { return new CheckNode(id); }},
        {"check+", [](unsigned id) { return new CheckNode(id, true); }},
        {"action", [](unsigned id) { return new ActionNode(id); }},
        {"action+", [](unsigned id) { return new ActionNode(id); }},
    };
}
