#include <iostream>

#include "../src/flow_capacitated_networks.hpp"

int main()
{
    FlowCapacitatedNetwork network1 = FlowCapacitatedNetwork::fromEdgeCapacitated(
        { "S", "A", "B", "C", "D", "T" },
        "S",
        "T",
        {
            Edge("S", "A", 2),
            Edge("S", "C", 4),
            Edge("A", "B", 3),
            Edge("A", "C", 1),
            Edge("B", "C", 3),
            Edge("B", "T", 4),
            Edge("C", "D", 3),
            Edge("D", "B", 1),
            Edge("D", "T", 3)
        }
    );
    
    FlowCapacitatedNetwork network2 = FlowCapacitatedNetwork::fromVertexCapacitated(
        { "S", "A", "B", "C", "D", "T" },
        "S",
        "T",
        {
            { "S", "A" },
            { "S", "B" },

            { "A", "C" },
            { "B", "D" },

            { "C", "T" },
            { "D", "T" },
        },
        {
            { "A", 2 },
            { "B", 5 },
            { "C", 3 },
            { "D", 4 }
        }
    );

    FlowCapacitatedNetwork network3 = FlowCapacitatedNetwork::fromEdgeAndVertexCapacitated(
        { "S", "A", "B", "C", "D", "T" },
        "S",
        "T",
        {
            Edge("S", "A", 1),
            Edge("S", "B", 1),

            Edge("A", "C", 1),
            Edge("B", "D", 1),

            Edge("C", "T", 1),
            Edge("D", "T", 1)
        },
        {
            { "A", 2 },
            { "B", 5 },
            { "C", 3 },
            { "D", 4 }
        }
    );

    network3.maximizeFlow();

    std::cout << "flow: " << network3.getFlow();
};