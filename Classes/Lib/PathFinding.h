#pragma once

#include "CoreLib.h"
#include "PriorityQueue.h"

namespace lib
{

template<typename T>
struct Graph {
    typedef T Node;
    
    inline const std::vector<T> neighbors(T id) = 0;
    inline int cost(T t1, T t2) = 0;
    inline int heuristic(T t1, T t2) = 0;
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
        
        while (!frontier.empty())
        {
            auto current = frontier.front();
            frontier.pop();
            
            if (current == goal)
                break;
            
            for (auto next : graph.neighbors(current))
            {
                if (!cameFrom.count(next))
                {
                    frontier.push(next);
                    cameFrom[next] = current;
                }
            }
        }
        
        if (frontier.empty())
            return nullptr;
        
        return reconstruct(cameFrom, start, goal);
    }
    
    template<typename Graph>
    static lib::Nullable<std::vector<typename Graph::Node>> dijkstraSearch(
            Graph& graph, typename Graph::Node start, typename Graph::Node goal)
    {
        typedef typename Graph::Node Node;
        
        std::map<Node, Node> cameFrom;
        std::map<Node, int> costSoFar;
        
        PriorityQueue<Node> frontier;
        frontier.put(start, 0);
        
        cameFrom[start] = start;
        costSoFar[start] = 0;
        
        while (!frontier.empty())
        {
            auto current = frontier.get();
            
            if (current == goal)
                break;
            
            for (auto next : graph.neighbors(current))
            {
                int newCost = costSoFar[current] + graph.cost(current, next);
                if (!costSoFar.count(next) || newCost < costSoFar[next]) {
                    costSoFar[next] = newCost;
                    cameFrom[next] = current;
                    frontier.put(next, newCost);
                }
            }
        }
        
        if (frontier.empty())
            return nullptr;
        
        return reconstruct(cameFrom, start, goal);
    }
    
    template<typename Graph>
    static lib::Nullable<std::list<typename Graph::Node>> aStarSearch(
            Graph& graph, typename Graph::Node start, typename Graph::Node goal)
    {
        typedef typename Graph::Node Node;
        
        std::map<Node, Node> cameFrom;
        std::map<Node, int> costSoFar;
        
        PriorityQueue<Node> frontier;
        frontier.put(start, 0);
        
        cameFrom[start] = start;
        costSoFar[start] = 0;
        
        while (!frontier.empty())
        {
            auto current = frontier.get();
            
            if (current == goal)
                break;
            
            for (auto next : graph.neighbors(current))
            {
                int newCost = costSoFar[current] + graph.cost(current, next);
                if (!costSoFar.count(next) || newCost < costSoFar[next])
                {
                    costSoFar[next] = newCost;
                    int priority = newCost + graph.heuristic(next, goal);
                    frontier.put(next, priority);
                    cameFrom[next] = current;
                }
            }
        }
        
        if (frontier.empty())
            return nullptr;
        
        return reconstruct(cameFrom, start, goal);
    }

private:
    template<typename Node>
    static std::list<Node> reconstruct(std::map<Node, Node>& cameFrom,
                                         const Node& start, const Node& goal)
    {
        std::list<Node> result;
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


}