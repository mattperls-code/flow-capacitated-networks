#include <iostream>

#include "../src/flow_capacitated_networks.hpp"

int main()
{
    FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromEdgeCapacitated(
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

    std::cout << "Initial Network:" << std::endl << network.toString();
    std::cout << "Flow: " << network.getFlow() << std::endl << std::endl;

    network.maximizeFlow();

    std::cout << "Final Network:" << std::endl << network.toString();
    std::cout << "Flow: " << network.getFlow() << std::endl << std::endl;
};