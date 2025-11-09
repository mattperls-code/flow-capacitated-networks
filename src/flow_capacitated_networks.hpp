#ifndef FLOW_CAPACITATED_NETWORKS
#define FLOW_CAPACITATED_NETWORKS

#include <string>
#include <unordered_set>
#include <unordered_map>

class Edge
{
    public:
        std::string start;
        std::string end;
        int capacity;

        Edge(std::string start, std::string end, int capacity): start(start), end(end), capacity(capacity) {};

        bool operator==(const Edge&) const = default;
};

template <>
struct std::hash<Edge> {
    size_t operator()(const Edge& edge) const;
};

template <>
struct std::hash<std::pair<std::string, std::string>> {
    size_t operator()(const std::pair<std::string, std::string>& edge) const;
};

class FlowCapacitatedNetwork
{
    private:
        std::unordered_set<std::string> nodes;

        std::string source;
        std::string terminal;

        // matrix[start][end] = value of edge from start to end
        std::unordered_map<std::string, std::unordered_map<std::string, int>> capacityMatrix;
        std::unordered_map<std::string, std::unordered_map<std::string, int>> flowMatrix;
        std::unordered_map<std::string, std::unordered_map<std::string, int>> residualMatrix;

        FlowCapacitatedNetwork(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<Edge> edges);

    public:
        static FlowCapacitatedNetwork fromEdgeCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<Edge> edges);
        static FlowCapacitatedNetwork fromVertexCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<std::pair<std::string, std::string>> edges, std::unordered_map<std::string, int> vertexCapacity);
        static FlowCapacitatedNetwork fromEdgeAndVertexCapacitated(std::unordered_set<std::string> nodes, std::string source, std::string terminal, std::unordered_set<Edge> edges, std::unordered_map<std::string, int> vertexCapacity);
        
        static FlowCapacitatedNetwork fromMultiBoundaryEdgeCapacitated(std::unordered_set<std::string> nodes, std::unordered_set<std::string> sources, std::unordered_set<std::string> terminals, std::unordered_set<Edge> edges);
        static FlowCapacitatedNetwork fromMultiBoundaryVertexCapacitated(std::unordered_set<std::string> nodes, std::unordered_set<std::string> sources, std::unordered_set<std::string> terminals, std::unordered_set<std::pair<std::string, std::string>> edges, std::unordered_map<std::string, int> vertexCapacity);
        static FlowCapacitatedNetwork fromMultiBoundaryEdgeAndVertexCapacitated(std::unordered_set<std::string> nodes, std::unordered_set<std::string> sources, std::unordered_set<std::string> terminals, std::unordered_set<Edge> edges, std::unordered_map<std::string, int> vertexCapacity);

        int getFlow();

        std::pair<std::unordered_set<std::string>, std::unordered_set<std::string>> findMinCut();

        bool isMaxFlow();

        void augment();
        void maximizeFlow();

        std::string toString();

        std::string capacityGraphToDOT();
        std::string flowGraphToDOT();
        std::string residualGraphToDOT();

        void exportCapacityGraph(std::string outputDirPath, std::string outputFileName);
        void exportFlowGraph(std::string outputDirPath, std::string outputFileName);
        void exportResidualGraph(std::string outputDirPath, std::string outputFileName);
};

#endif