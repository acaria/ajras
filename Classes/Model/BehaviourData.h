#pragma once

namespace behaviour
{
    class BaseNode;
}

class BehaviourData
{
public:
    BehaviourData(const std::string& path);
    ~BehaviourData();
    
    behaviour::BaseNode* getNode(unsigned id);
    behaviour::BaseNode* getRootNode();
    
private:
    behaviour::BaseNode* tree = nullptr;
    std::map<unsigned, behaviour::BaseNode*> dict;
    
    unsigned curId;

    int getLineDepth(const std::string& line);
    void parsingNode(behaviour::BaseNode* node,
                     std::list<std::string>& lines);
};