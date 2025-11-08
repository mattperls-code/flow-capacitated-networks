#include <iostream>
#include <stdexcept>
#include <queue>

#include "flow_capacitated_networks.hpp"

size_t std::hash<Edge>::operator()(const Edge& edge) const
{
    size_t startHash = std::hash<std::string>()(edge.start);
    size_t endHash = std::hash<std::string>()(edge.end);

    return startHash ^ endHash ^ edge.capacity;
};

size_t std::hash<std::pair<std::string, std::string>>::operator()(const std::pair<std::string, std::string>& edge) const
{
    size_t startHash = std::hash<std::string>()(edge.first);
    size_t endHash = std::hash<std::string>()(edge.second);

    return startHash ^ endHash;
};

FlowCapacitatedNetwork::FlowCapacitatedNetwork(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<Edge> edges)
{
    this->nodes = nodes;
    this->source = source;
    this->terminal = terminal;
    
    for (const auto& edge : edges) {
        this->capacityMatrix[edge.start][edge.end] = edge.capacity;
        this->flowMatrix[edge.start][edge.end] = 0;
        this->residualMatrix[edge.start][edge.end] = edge.capacity;
        this->residualMatrix[edge.end][edge.start] = 0;
    }
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromEdgeCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<Edge> edges)
{
    if (!nodes.contains(source)) throw std::runtime_error("FlowCapacitatedNetwork fromEdgeCapacitated: nodes does not contain source");
    if (!nodes.contains(terminal)) throw std::runtime_error("FlowCapacitatedNetwork fromEdgeCapacitated: nodes does not contain terminal");

    for (const auto& node : nodes) if (node.empty()) throw std::runtime_error("FlowCapacitatedNetwork fromEdgeCapacitated: nodes must have a name");

    for (const auto& edge : edges) {
        if (!nodes.contains(edge.start) || !nodes.contains(edge.end)) throw std::runtime_error("FlowCapacitatedNetwork fromEdgeCapacitated: edge contains invalid node");
        
        if (edge.capacity < 0) throw std::runtime_error("FlowCapacitatedNetwork fromEdgeCapacitated: edge capacity cannot be negative");
    }

    return FlowCapacitatedNetwork(nodes, source, terminal, edges);
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromVertexCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<std::pair<std::string, std::string>> edges, std::unordered_map<std::string, int> vertexCapacity)
{
    if (vertexCapacity.contains(source) || vertexCapacity.contains(terminal)) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: source and terminal cannot have a capacity");

    std::unordered_set<std::string> splitNodes;
    splitNodes.insert(source + "-out");
    splitNodes.insert(terminal + "-in");

    int maxCapacity = 0;

    for (const auto& [node, capacity] : vertexCapacity) {
        if (!nodes.contains(node)) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: vertex capacity contains invalid node");

        if (node.contains('-')) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: node name cannot have dashes");

        if (capacity < 0) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: vertex capacity cannot be negative");

        splitNodes.insert(node + "-in");
        splitNodes.insert(node + "-out");

        maxCapacity += capacity;
    }

    if (vertexCapacity.size() != nodes.size() - 2) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: every internal node must have an explicit capacity");

    std::unordered_set<Edge> edgesWithCapacities;

    for (const auto& [start, end] : edges) edgesWithCapacities.emplace(start + "-out",  end + "-in", maxCapacity);

    for (const auto& [node, capacity] : vertexCapacity) {
        edgesWithCapacities.emplace(node + "-in", node + "-out", capacity);
        edgesWithCapacities.emplace(node + "-out", node + "-in", capacity);
    }

    return FlowCapacitatedNetwork::fromEdgeCapacitated(splitNodes, source + "-out", terminal + "-in", edgesWithCapacities);
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromEdgeAndVertexCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<Edge> edges, std::unordered_map<std::string, int> vertexCapacity)
{
    if (vertexCapacity.contains(source) || vertexCapacity.contains(terminal)) throw std::runtime_error("FlowCapacitatedNetwork fromEdgeAndVertexCapacitated: source and terminal cannot have a capacity");

    std::unordered_set<std::string> splitNodes;
    splitNodes.insert(source + "-out");
    splitNodes.insert(terminal + "-in");

    for (const auto& [node, capacity] : vertexCapacity) {
        if (!nodes.contains(node)) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: vertex capacity contains invalid node");

        if (node.contains('-')) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: node name cannot have dashes");

        if (capacity < 0) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: vertex capacity cannot be negative");

        splitNodes.insert(node + "-in");
        splitNodes.insert(node + "-out");
    }

    if (vertexCapacity.size() != nodes.size() - 2) throw std::runtime_error("FlowCapacitatedNetwork fromVertexCapacitated: every internal node must have an explicit capacity");

    std::unordered_set<Edge> edgesWithCapacities;

    for (const auto& edge : edges) edgesWithCapacities.emplace(edge.start + "-out",  edge.end + "-in", edge.capacity);

    for (const auto& [node, capacity] : vertexCapacity) {
        edgesWithCapacities.emplace(node + "-in", node + "-out", capacity);
        edgesWithCapacities.emplace(node + "-out", node + "-in", capacity);
    }

    return FlowCapacitatedNetwork::fromEdgeCapacitated(splitNodes, source + "-out", terminal + "-in", edgesWithCapacities);
};

int FlowCapacitatedNetwork::getFlow()
{
    int sum = 0;

    for (auto [_, flow] : this->flowMatrix[this->source]) sum += flow;

    return sum;
};

std::pair<std::unordered_set<std::string>, std::unordered_set<std::string>> FlowCapacitatedNetwork::findMinCut()
{
    std::unordered_set<std::string> reachableFromSource;
    std::unordered_set<std::string> unreachableFromSource = this->nodes;

    std::queue<std::string> queue;
    queue.push(this->source);

    while (!queue.empty()) {
        std::string currNode = queue.front();
        queue.pop();

        reachableFromSource.insert(currNode);
        unreachableFromSource.erase(currNode);

        for (const auto& [neighbor, flow] : this->residualMatrix[currNode]) if (!reachableFromSource.contains(neighbor) && flow > 0) queue.push(neighbor);
    }

    return { reachableFromSource, unreachableFromSource };
};

bool FlowCapacitatedNetwork::isMaxFlow()
{
    return this->findMinCut().first.contains(this->terminal);
};

void FlowCapacitatedNetwork::augment()
{
    std::unordered_map<std::string, std::string> parentTable;
    parentTable[this->source] = "";
    
    std::queue<std::string> queue;
    queue.push(this->source);

    std::string currNode;

    while (!queue.empty()) {
        currNode = queue.front();
        queue.pop();
        
        for (const auto& [neighbor, flow] : this->residualMatrix[currNode]) if (!parentTable.contains(neighbor) && flow > 0) {
            parentTable[neighbor] = currNode;
            queue.push(neighbor);
        }
    }

    if (!parentTable.contains(this->terminal)) throw std::runtime_error("FlowCapacitatedNetwork augment: network is already maximal");

    int bottleneck = this->residualMatrix[parentTable[this->terminal]][this->terminal];

    currNode = this->terminal;

    while (!parentTable[currNode].empty()) {
        std::string parent = parentTable[currNode];

        if (this->residualMatrix[parent][currNode] < bottleneck) bottleneck = this->residualMatrix[parent][currNode];

        currNode = parent;
    }

    currNode = this->terminal;

    while (!parentTable[currNode].empty()) {
        std::string parent = parentTable[currNode];

        this->residualMatrix[parent][currNode] -= bottleneck;
        this->residualMatrix[currNode][parent] += bottleneck;

        if (this->flowMatrix[parent].contains(currNode)) this->flowMatrix[parent][currNode] += bottleneck;
        else this->flowMatrix[currNode][parent] -= bottleneck;

        currNode = parent;
    }
};

void FlowCapacitatedNetwork::maximizeFlow()
{
    while (true) {
        std::unordered_map<std::string, std::string> parentTable;
        parentTable[this->source] = "";
        
        std::queue<std::string> queue;
        queue.push(this->source);

        std::string currNode;

        while (!queue.empty()) {
            currNode = queue.front();
            queue.pop();
            
            for (const auto& [neighbor, flow] : this->residualMatrix[currNode]) if (!parentTable.contains(neighbor) && flow > 0) {
                parentTable[neighbor] = currNode;
                queue.push(neighbor);
            }
        }

        if (!parentTable.contains(this->terminal)) break;

        int bottleneck = this->residualMatrix[parentTable[this->terminal]][this->terminal];

        currNode = this->terminal;

        while (!parentTable[currNode].empty()) {
            std::string parent = parentTable[currNode];

            if (this->residualMatrix[parent][currNode] < bottleneck) bottleneck = this->residualMatrix[parent][currNode];

            currNode = parent;
        }

        currNode = this->terminal;

        while (!parentTable[currNode].empty()) {
            std::string parent = parentTable[currNode];

            this->residualMatrix[parent][currNode] -= bottleneck;
            this->residualMatrix[currNode][parent] += bottleneck;

            if (this->flowMatrix[parent].contains(currNode)) this->flowMatrix[parent][currNode] += bottleneck;
            else this->flowMatrix[currNode][parent] -= bottleneck;

            currNode = parent;
        }
    }
};

std::string FlowCapacitatedNetwork::toString()
{
    std::string output;

    output += "Nodes: ";

    for (const auto& node : this->nodes) output += node + ", ";

    output.pop_back();
    output.pop_back();

    output += "\n";

    output += "Source: " + this->source + "\n";
    output += "Terminal: " + this->terminal + "\n";

    output += "Capacity Matrix:\n";

    for (const auto& [start, neighbors] : this->capacityMatrix) {
        for (const auto& [end, capacity] : neighbors) output += "\t C(" + start + ", " + end + ") = " + std::to_string(capacity) + "\n";
    }

    output += "Flow Matrix:\n";

    for (const auto& [start, neighbors] : this->flowMatrix) {
        for (const auto& [end, flow] : neighbors) output += "\t F(" + start + ", " + end + ") = " + std::to_string(flow) + "\n";
    }

    output += "Residual Flow Matrix:\n";

    for (const auto& [start, neighbors] : this->residualMatrix) {
        for (const auto& [end, residualFlow] : neighbors) output += "\t R(" + start + ", " + end + ") = " + std::to_string(residualFlow) + "\n";
    }

    return output;
};