#pragma once

#include "CoreLib.h"

template<typename T>
struct Graph {
    typedef T Node;
    typedef typename std::vector<T>::iterator iterator;
    std::map<T, std::vector<T>> edges;
    
    inline const std::vector<T> neighbors(T id) {
        return edges[id];
    }
};

class PathFinding
{
public:
    template<typename Graph>
    static lib::Nullable<std::vector<typename Graph::Node>> breadthFirstSearch(
            Graph& graph, typename Graph::Node start, typename Graph::Node goal) {
        typedef typename Graph::Node Node;
        std::queue<Node> frontier;
        frontier.push(start);
        
        std::map<Node, Node> cameFrom;
        cameFrom[start] = start;
        
        while (!frontier.empty()) {
            auto current = frontier.front();
            frontier.pop();
            
            if (current == goal) {
                break;
            }
            
            for (auto next : graph.neighbors(current)) {
                if (!cameFrom.count(next)) {
                    frontier.push(next);
                    cameFrom[next] = current;
                }
            }
        }
        
        if (frontier.empty())
            return nullptr;
        
        std::vector<Node> result;
        Node current = goal;
        while(current != start)
        {
            result.push_back(current);
            current = cameFrom[current];
        }
        
        std::reverse(result.begin(), result.end());
        
        return result;
    }
};