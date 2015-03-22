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
            runningNode = nullptr;
            this->onCheck = nullptr;
            this->onExecute = nullptr;
        }
        
        void reset()
        {
            runningNode = nullptr;
            states.clear();
        }
        
        std::map<std::string, cc::Value>& getFields(unsigned id)
        {
            if (fields.find(id) == fields.end())
                fields[id] = std::map<std::string, cc::Value>();
            return fields[id];
        }
        
        std::map<unsigned, nState> states;
        
        BaseNode* runningNode;
        
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
                    board.runningNode = this;
                    this->recursiveClearStates(this, board);
                }
                else //(board.states[id] == FAILURE)
                {
                    board.states[id] = FAILURE;
                    return FAILURE;
                }
            }
            board.states[id] = RUNNING;
            
            return this->children[0]->visit(board);
        }
        
    private:
        void recursiveClearStates(BaseNode* node, BoardNode& board)
        {
            board.states.erase(node->id);
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
            Log("SequenceNode: %s", this->name.c_str());
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
            Log("SelectorNode: %s", this->name.c_str());
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
            Log("UntilNode: %s", this->name.c_str());
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
                board.runningNode = this;
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
        CheckNode(unsigned id) : BaseNode(id) {}
    
        nState visit(BoardNode& board)
        {
#ifdef kTraceBehaviours
            Log("CheckNode: %s", this->name.c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
        
            board.states[this->id] = board.onCheck(this->id);
            return board.states[this->id];
        }
    };

    struct ActionNode : public BaseNode
    {
        ActionNode(unsigned id) : BaseNode(id) {}
    
        nState visit(BoardNode& board)
        {
#ifdef kTraceBehaviours
            Log("ActionNode: %s", this->name.c_str());
#endif
            if (board.states.find(id) != board.states.end() && board.states[id] != RUNNING)
                return board.states[id];
        
            auto state = board.onExecute(this->id);
            
            if (state == RUNNING)
                board.runningNode = this;
            board.states[this->id] = state;
            return state;
        }
    };
    
    static std::map<std::string, std::function<BaseNode*(unsigned id)>> factory {
        {"selector", [](unsigned id) { return new SelectorNode(id); }},
        {"sequence", [](unsigned id) { return new SequenceNode(id); }},
        {"repeat", [](unsigned id) { return new RepeatNode(id); }},
        {"until", [](unsigned id) { return new UntilNode(id); }},
        {"check", [](unsigned id) { return new CheckNode(id); }},
        {"action", [](unsigned id) { return new ActionNode(id); }}
    };
}
