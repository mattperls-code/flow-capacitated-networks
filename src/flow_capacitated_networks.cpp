#include <iostream>
#include <stdexcept>
#include <queue>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <fstream>

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

int getMaxCapacity(std::unordered_map<std::string, int> vertexCapacity)
{
    int maxCapacity = 0;

    for (const auto& [_, capacity] : vertexCapacity) maxCapacity = std::max(maxCapacity, capacity);

    return maxCapacity;
};

int getMaxCapacity(std::unordered_set<Edge> edges)
{
    int maxCapacity = 0;

    for (const auto& edge : edges) maxCapacity = std::max(maxCapacity, edge.capacity);

    return maxCapacity;
};

int getMaxCapacity(std::unordered_map<std::string, int> vertexCapacity, std::unordered_set<Edge> edges)
{
    return std::max(getMaxCapacity(vertexCapacity), getMaxCapacity(edges));
};

int getTotalCapacity(std::unordered_map<std::string, int> vertexCapacity)
{
    int totalCapacity = 0;

    for (const auto& [_, capacity] : vertexCapacity) totalCapacity += capacity;

    return totalCapacity;
};

int getTotalCapacity(std::unordered_set<Edge> edges)
{
    int totalCapacity = 0;

    for (const auto& edge : edges) totalCapacity += edge.capacity;

    return totalCapacity;
};

int getTotalCapacity(std::unordered_map<std::string, int> vertexCapacity, std::unordered_set<Edge> edges)
{
    return getTotalCapacity(vertexCapacity) + getTotalCapacity(edges);
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
        if (!this->residualMatrix[edge.end].contains(edge.start)) this->residualMatrix[edge.end][edge.start] = 0;
    }
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromEdgeCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<Edge> edges)
{
    if (!nodes.contains(source)) throw std::runtime_error("FlowCapacitatedNetwork constructor: nodes does not contain source");
    if (!nodes.contains(terminal)) throw std::runtime_error("FlowCapacitatedNetwork constructor: nodes does not contain terminal");

    for (const auto& node : nodes) if (node.empty()) throw std::runtime_error("FlowCapacitatedNetwork constructor: nodes must have a name");

    for (const auto& edge : edges) {
        if (!nodes.contains(edge.start) || !nodes.contains(edge.end)) throw std::runtime_error("FlowCapacitatedNetwork constructor: edge contains invalid node");

        if (edge.end == source) throw std::runtime_error("FlowCapacitatedNetwork constructor: edge cannot end at source");
        if (edge.start == terminal) throw std::runtime_error("FlowCapacitatedNetwork constructor: edge cannot start at terminal");
        
        if (edge.capacity < 0) throw std::runtime_error("FlowCapacitatedNetwork constructor: edge capacity cannot be negative");
    }

    return FlowCapacitatedNetwork(nodes, source, terminal, edges);
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromVertexCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<std::pair<std::string, std::string>> edges, std::unordered_map<std::string, int> vertexCapacity)
{
    int maxCapacity = getMaxCapacity(vertexCapacity);

    std::unordered_set<Edge> edgesWithCapacities;

    for (const auto& [start, end] : edges) edgesWithCapacities.emplace(start, end, maxCapacity);

    return fromEdgeAndVertexCapacitated(nodes, source, terminal, edgesWithCapacities, vertexCapacity);
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromEdgeAndVertexCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<Edge> edges, std::unordered_map<std::string, int> vertexCapacity)
{
    if (vertexCapacity.contains(source) || vertexCapacity.contains(terminal)) throw std::runtime_error("FlowCapacitatedNetwork constructor: source and terminal cannot have a capacity");

    std::unordered_set<std::string> splitNodes;
    splitNodes.emplace(source + "-out");
    splitNodes.emplace(terminal + "-in");

    for (const auto& [node, capacity] : vertexCapacity) {
        if (!nodes.contains(node)) throw std::runtime_error("FlowCapacitatedNetwork constructor: vertex capacity contains invalid node");

        if (node.contains('-')) throw std::runtime_error("FlowCapacitatedNetwork constructor: node name cannot have dashes");

        if (capacity < 0) throw std::runtime_error("FlowCapacitatedNetwork constructor: vertex capacity cannot be negative");

        splitNodes.emplace(node + "-in");
        splitNodes.emplace(node + "-out");
    }

    if (vertexCapacity.size() != nodes.size() - 2) throw std::runtime_error("FlowCapacitatedNetwork constructor: every internal node must have an explicit capacity");

    std::unordered_set<Edge> edgesWithCapacities;

    for (const auto& edge : edges) edgesWithCapacities.emplace(edge.start + "-out",  edge.end + "-in", edge.capacity);

    for (const auto& [node, capacity] : vertexCapacity) {
        edgesWithCapacities.emplace(node + "-in", node + "-out", capacity);
        edgesWithCapacities.emplace(node + "-out", node + "-in", capacity);
    }

    return fromEdgeCapacitated(splitNodes, source + "-out", terminal + "-in", edgesWithCapacities);
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromMultiBoundaryEdgeCapacitated(std::unordered_set<std::string> nodes, std::unordered_set<std::string> sources, std::unordered_set<std::string> terminals, std::unordered_set<Edge> edges)
{
    for (const auto& node : nodes) if (node.contains("$")) throw std::runtime_error("FlowCapacitatesNetwork constructor: node name cannot have $");

    nodes.emplace("$S");
    nodes.emplace("$T");

    int totalCapacity = getTotalCapacity(edges);

    for (const auto& source : sources) edges.emplace("$S", source, totalCapacity);
    for (const auto& terminal : terminals) edges.emplace(terminal, "$T", totalCapacity);

    return fromEdgeCapacitated(nodes, "$S", "$T", edges);
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromMultiBoundaryVertexCapacitated(std::unordered_set<std::string> nodes, std::unordered_set<std::string> sources, std::unordered_set<std::string> terminals, std::unordered_set<std::pair<std::string, std::string>> edges, std::unordered_map<std::string, int> vertexCapacity)
{
    for (const auto& node : nodes) if (node.contains("$")) throw std::runtime_error("FlowCapacitatesNetwork constructor: node name cannot have $");

    nodes.emplace("$S");
    nodes.emplace("$T");

    int totalCapacity = getTotalCapacity(vertexCapacity);

    for (const auto& source : sources) {
        edges.emplace("$S", source);
        vertexCapacity[source] = totalCapacity;
    }

    for (const auto& terminal : terminals) {
        edges.emplace(terminal, "$T");
        vertexCapacity[terminal] = totalCapacity;
    }

    return fromVertexCapacitated(nodes, "$S", "$T", edges, vertexCapacity);
};

FlowCapacitatedNetwork FlowCapacitatedNetwork::fromMultiBoundaryEdgeAndVertexCapacitated(std::unordered_set<std::string> nodes, std::unordered_set<std::string> sources, std::unordered_set<std::string> terminals, std::unordered_set<Edge> edges, std::unordered_map<std::string, int> vertexCapacity)
{
    for (const auto& node : nodes) if (node.contains("$")) throw std::runtime_error("FlowCapacitatesNetwork constructor: node name cannot have $");

    nodes.emplace("$S");
    nodes.emplace("$T");

    int totalCapacity = getTotalCapacity(vertexCapacity, edges);

    for (const auto& source : sources) {
        edges.emplace("$S", source, totalCapacity);
        vertexCapacity[source] = totalCapacity;
    }

    for (const auto& terminal : terminals) {
        edges.emplace(terminal, "$T", totalCapacity);
        vertexCapacity[terminal] = totalCapacity;
    }

    return fromEdgeAndVertexCapacitated(nodes, "$S", "$T", edges, vertexCapacity);
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

        reachableFromSource.emplace(currNode);
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

bool alphanumLess(const std::string& a, const std::string& b) {
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (std::isdigit(a[i]) && std::isdigit(b[j])) {
            size_t ia = i, ib = j;
            while (ia < a.size() && std::isdigit(a[ia])) ++ia;
            while (ib < b.size() && std::isdigit(b[ib])) ++ib;
            
            int numA = std::stoi(a.substr(i, ia - i));
            int numB = std::stoi(b.substr(j, ib - j));
            
            if (numA != numB) return numA < numB;
            i = ia;
            j = ib;
        } else {
            if (a[i] != b[j]) return a[i] < b[j];
            ++i;
            ++j;
        }
    }
    return a.size() < b.size();
}

void alphanumSort(std::vector<std::string>& v) {
    std::sort(v.begin(), v.end(), alphanumLess);
}

std::string FlowCapacitatedNetwork::toString()
{
    std::string output;

    output += "Nodes: ";

    std::vector<std::string> sortedNodes;

    for (const auto& node : this->nodes) sortedNodes.emplace_back(node + ", ");

    alphanumSort(sortedNodes);

    for (const auto& node : sortedNodes) output += node;

    output.pop_back();
    output.pop_back();

    output += "\n";

    output += "Source: " + this->source + "\n";
    output += "Terminal: " + this->terminal + "\n";

    output += "Capacity Matrix:\n";

    std::vector<std::string> sortedCapacityMatrix;

    for (const auto& [start, neighbors] : this->capacityMatrix) {
        for (const auto& [end, capacity] : neighbors) sortedCapacityMatrix.emplace_back("\tC(" + start + ", " + end + ") = " + std::to_string(capacity) + "\n");
    }

    alphanumSort(sortedCapacityMatrix);

    for (const auto& capacity : sortedCapacityMatrix) output += capacity;

    // output += "Flow Matrix:\n";
    
    // std::vector<std::string> sortedFlowMatrix;

    // for (const auto& [start, neighbors] : this->flowMatrix) {
    //     for (const auto& [end, capacity] : neighbors) sortedFlowMatrix.emplace_back("\t F(" + start + ", " + end + ") = " + std::to_string(capacity) + "\n");
    // }

    // alphanumSort(sortedFlowMatrix);

    // for (const auto& capacity : sortedFlowMatrix) output += capacity;

    // output += "Residual Flow Matrix:\n";
    
    // std::vector<std::string> sortedResidualMatrix;

    // for (const auto& [start, neighbors] : this->residualMatrix) {
    //     for (const auto& [end, capacity] : neighbors) sortedResidualMatrix.emplace_back("\t R(" + start + ", " + end + ") = " + std::to_string(capacity) + "\n");
    // }

    // alphanumSort(sortedResidualMatrix);

    // for (const auto& capacity : sortedResidualMatrix) output += capacity;

    return output;
};

std::string concatStrSet(std::unordered_set<std::string> strSet, std::string delimiter)
{
    std::string concat;
    
    for (auto str : strSet) concat += str + delimiter;
    
    if (!strSet.empty()) concat = concat.substr(0, concat.size() - delimiter.size());

    return concat;
};

std::string FlowCapacitatedNetwork::capacityGraphToDOT()
{
    std::string output;

    output += "digraph CapacityGraph {";
    output += "\n";

    output += "\trankdir=LR;";
    output += "\n";
    output += "\tnodesep=1.0;";
    output += "\n";
    output += "\tranksep=1.0;";
    output += "\n";

    output += "\t\"" + source + "\" [shape=circle, style=filled, fillcolor=lightblue, penwidth=3, fontsize=20];\n";
    output += "\t\"" + terminal + "\" [shape=circle, style=filled, fillcolor=lightcoral, penwidth=3, fontsize=20];\n";

    output += "\t{ rank=min; \"" + source + "\"; }\n";
    output += "\t{ rank=max; \"" + terminal + "\"; }\n";

    std::unordered_set<std::string> nodeDecls;
    for (const auto& node : nodes) if (node != source && node != terminal) nodeDecls.insert("\t\"" + node + "\" [shape=circle, fontsize=20];");

    output += concatStrSet(nodeDecls, "\n");
    output += "\n";

    std::unordered_set<std::string> edgeDotSet;
    for (const auto& [start, edges] : this->capacityMatrix) {
        for (auto& [end, capacity] : edges) {
            if (capacity > 0) edgeDotSet.insert("\t\"" + start + "\" -> \"" + end + "\" [label=\"" + std::to_string(capacity) + "\", fontsize=20];");
        }
    }
    output += concatStrSet(edgeDotSet, "\n");

    output += "\n";
    output += "}";

    return output;
};

std::string FlowCapacitatedNetwork::flowGraphToDOT()
{
    std::string output;

    output += "digraph FlowGraph {";
    output += "\n";

    output += "\trankdir=LR;";
    output += "\n";
    output += "\tnodesep=1.0;";
    output += "\n";
    output += "\tranksep=1.0;";
    output += "\n";

    output += "\t\"" + source + "\" [shape=circle, style=filled, fillcolor=lightblue, penwidth=3, fontsize=20];\n";
    output += "\t\"" + terminal + "\" [shape=circle, style=filled, fillcolor=lightcoral, penwidth=3, fontsize=20];\n";

    output += "\t{ rank=min; \"" + source + "\"; }\n";
    output += "\t{ rank=max; \"" + terminal + "\"; }\n";

    std::unordered_set<std::string> nodeDecls;
    for (const auto& node : nodes) if (node != source && node != terminal) nodeDecls.insert("\t\"" + node + "\" [shape=circle, fontsize=20];");

    output += concatStrSet(nodeDecls, "\n");
    output += "\n";

    std::unordered_set<std::string> edgeDotSet;
    for (const auto& [start, edges] : this->capacityMatrix) {
        for (auto& [end, capacity] : edges) {
            if (capacity > 0) edgeDotSet.insert("\t\"" + start + "\" -> \"" + end + "\" [label=\"" + std::to_string(this->flowMatrix[start][end]) + "/" + std::to_string(capacity) + "\", fontsize=20];");
        }
    }
    output += concatStrSet(edgeDotSet, "\n");

    output += "\n";
    output += "}";

    return output;
};

std::string FlowCapacitatedNetwork::residualGraphToDOT()
{
    std::string output;

    output += "digraph ResidualGraph {";
    output += "\n";

    output += "\trankdir=LR;";
    output += "\n";
    output += "\tnodesep=1.0;";
    output += "\n";
    output += "\tranksep=1.0;";
    output += "\n";

    output += "\t\"" + source + "\" [shape=circle, style=filled, fillcolor=lightblue, penwidth=3, fontsize=20];\n";
    output += "\t\"" + terminal + "\" [shape=circle, style=filled, fillcolor=lightcoral, penwidth=3, fontsize=20];\n";

    output += "\t{ rank=min; \"" + source + "\"; }\n";
    output += "\t{ rank=max; \"" + terminal + "\"; }\n";

    std::unordered_set<std::string> nodeDecls;
    for (const auto& node : nodes) if (node != source && node != terminal) nodeDecls.insert("\t\"" + node + "\" [shape=circle, fontsize=20];");

    output += concatStrSet(nodeDecls, "\n");
    output += "\n";

    std::unordered_set<std::string> edgeDotSet;
    for (const auto& [start, edges] : this->residualMatrix) {
        for (auto& [end, flow] : edges) {
            if (flow > 0) edgeDotSet.insert("\t\"" + start + "\" -> \"" + end + "\" [label=\"" + std::to_string(flow) + "\", fontsize=20];");
        }
    }
    output += concatStrSet(edgeDotSet, "\n");

    output += "\n";
    output += "}";

    return output;
};

void exportGraph(std::string outputDirPath, std::string outputFileName, std::string dotFileStr)
{
    std::filesystem::create_directories(outputDirPath);

    std::string dotOutputFilePath = outputDirPath + "/" + outputFileName + ".dot";

    std::ofstream dotOutputFile(dotOutputFilePath);

    dotOutputFile << dotFileStr;

    dotOutputFile.close();

    std::string renderDotFileCommand = "dot -Tpng " + dotOutputFilePath + " -o " + outputDirPath + "/" + outputFileName + ".png";

    std::system(renderDotFileCommand.c_str());
};

void FlowCapacitatedNetwork::exportCapacityGraph(std::string outputDirPath, std::string outputFileName)
{
    exportGraph(outputDirPath, outputFileName, this->capacityGraphToDOT());
};

void FlowCapacitatedNetwork::exportFlowGraph(std::string outputDirPath, std::string outputFileName)
{
    exportGraph(outputDirPath, outputFileName, this->flowGraphToDOT());
};

void FlowCapacitatedNetwork::exportResidualGraph(std::string outputDirPath, std::string outputFileName)
{
    exportGraph(outputDirPath, outputFileName, this->residualGraphToDOT());
};