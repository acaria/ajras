#include "BehaviourData.h"
#include "BehaviourNodes.h"
#include "CoreLib.h"

BehaviourData::BehaviourData(const std::string& path)
{
    std::string rawData = cc::FileUtils::getInstance()->getStringFromFile(path);
    
    std::list<std::string> lines;
    lib::split(rawData, lines, "\n", true);
    
    this->curId = 1;
    this->tree = new behaviour::RepeatNode(this->curId++, nullptr);
    this->dict[this->tree->id] = this->tree;
    this->parsingNode(this->tree, lines);
}

BehaviourData::~BehaviourData()
{
    if (tree != nullptr)
        delete tree;
}

int BehaviourData::getLineDepth(const std::string& line)
{
    int count = 0;
    for(int i = 0; i < line.length(); i++)
    {
        if (line[i] == '\t')
            count += 4;
        else if (line[i] == ' ')
            count++;
        else
            return count / 4;
    }
    return 0;
}

behaviour::BaseNode* BehaviourData::getRootNode()
{
    return this->tree;
}

behaviour::BaseNode* BehaviourData::getNode(unsigned id)
{
    return this->dict[id];
}

void BehaviourData::parsingNode(behaviour::BaseNode *node,
                                std::list<std::string>& lines)
{
    if (lines.size() == 0)
        return;
    auto line = lines.front();
    
    auto curDepth = getLineDepth(line);
    
    while (lines.size() > 0 && curDepth == getLineDepth(line))
    {
        lines.pop_front();
        if (lib::trimCopy(line)[0] != '#') //skip
        {
            std::vector<std::string> words;
            lib::split(line, words, "\t ", true);
            auto tagName = words[0];
    
            auto subNode = behaviour::factory[tagName](this->curId++, node);
            this->dict[subNode->id] = subNode;
            if (words.size() > 1)
                subNode->name = words[1];
            if (words.size() > 2)
                subNode->values = std::vector<std::string>(words.begin() + 2,
                                                       words.end());
        
            node->children.push_back(subNode);
        
            line = lines.front();
            if (getLineDepth(line) == curDepth + 1)
                this->parsingNode(subNode, lines);
        }
        if (lines.size() > 0)
            line = lines.front();
    }
}