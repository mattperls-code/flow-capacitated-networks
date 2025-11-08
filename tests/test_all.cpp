#include <catch2/catch_all.hpp>

#include "../src/flow_capacitated_networks.hpp"

TEST_CASE("CONSTRUCTIONS") {
    SECTION("EDGE CAPACITATED") {
        FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromEdgeCapacitated(
            { "S", "A", "B", "C", "D", "E", "T" },
            "S",
            "T",
            {
                Edge("S", "A", 5),
                Edge("S", "B", 3),
                Edge("S", "C", 4),
                Edge("A", "D", 6),
                Edge("B", "A", 2),
                Edge("B", "D", 2),
                Edge("C", "E", 5),
                Edge("D", "C", 3),
                Edge("D", "E", 2),
                Edge("D", "T", 5),
                Edge("E", "D", 4),
                Edge("E", "T", 6),
            }
        );

        std::string observedNetworkStructure = network.toString();
        std::string expectedNetworkStructure =
        "Nodes: A, B, C, D, E, S, T\n"
        "Source: S\n"
        "Terminal: T\n"
        "Capacity Matrix:\n"
        "\tC(A, D) = 6\n"
        "\tC(B, A) = 2\n"
        "\tC(B, D) = 2\n"
        "\tC(C, E) = 5\n"
        "\tC(D, C) = 3\n"
        "\tC(D, E) = 2\n"
        "\tC(D, T) = 5\n"
        "\tC(E, D) = 4\n"
        "\tC(E, T) = 6\n"
        "\tC(S, A) = 5\n"
        "\tC(S, B) = 3\n"
        "\tC(S, C) = 4\n";

        REQUIRE(observedNetworkStructure == expectedNetworkStructure);
    }

    SECTION("VERTEX CAPACITATED") {
        FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromVertexCapacitated(
            { "S", "A", "B", "C", "D", "E", "T" },
            "S",
            "T",
            {
                { "S", "A" },
                { "S", "B" },
                { "A", "C" },
                { "A", "D" },
                { "B", "D" },
                { "B", "E" },
                { "C", "T" },
                { "D", "T" },
                { "E", "A" },
                { "E", "T" },
            },
            {
                { "A", 4 },
                { "B", 5 },
                { "C", 2 },
                { "D", 3 },
                { "E", 3 },
            }
        );
        
        std::string observedNetworkStructure = network.toString();
        std::string expectedNetworkStructure =
        "Nodes: A-in, A-out, B-in, B-out, C-in, C-out, D-in, D-out, E-in, E-out, S-out, T-in\n"
        "Source: S-out\n"
        "Terminal: T-in\n"
        "Capacity Matrix:\n"
        "\tC(A-in, A-out) = 4\n"
        "\tC(A-out, A-in) = 4\n"
        "\tC(A-out, C-in) = 5\n"
        "\tC(A-out, D-in) = 5\n"
        "\tC(B-in, B-out) = 5\n"
        "\tC(B-out, B-in) = 5\n"
        "\tC(B-out, D-in) = 5\n"
        "\tC(B-out, E-in) = 5\n"
        "\tC(C-in, C-out) = 2\n"
        "\tC(C-out, C-in) = 2\n"
        "\tC(C-out, T-in) = 5\n"
        "\tC(D-in, D-out) = 3\n"
        "\tC(D-out, D-in) = 3\n"
        "\tC(D-out, T-in) = 5\n"
        "\tC(E-in, E-out) = 3\n"
        "\tC(E-out, A-in) = 5\n"
        "\tC(E-out, E-in) = 3\n"
        "\tC(E-out, T-in) = 5\n"
        "\tC(S-out, A-in) = 5\n"
        "\tC(S-out, B-in) = 5\n";

        REQUIRE(observedNetworkStructure == expectedNetworkStructure);
    }

    SECTION("EDGE AND VERTEX CAPACITATED") {
        FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromEdgeAndVertexCapacitated(
            { "S", "A", "B", "C", "D", "E", "T" },
            "S",
            "T",
            {
                Edge("S", "A", 4),
                Edge("S", "B", 4),
                Edge("S", "C", 2),
                Edge("A", "B", 2),
                Edge("A", "D", 1),
                Edge("A", "E", 2),
                Edge("B", "D", 1),
                Edge("B", "T", 3),
                Edge("C", "T", 3),
                Edge("D", "E", 2),
                Edge("D", "T", 2),
                Edge("E", "C", 1),
                Edge("E", "D", 2),
                Edge("E", "T", 3)
            },
            {
                { "A", 2 },
                { "B", 5 },
                { "C", 4 },
                { "D", 3 },
                { "E", 3 },
            }
        );

        std::string observedNetworkStructure = network.toString();
        std::string expectedNetworkStructure =
        "Nodes: A-in, A-out, B-in, B-out, C-in, C-out, D-in, D-out, E-in, E-out, S-out, T-in\n"
        "Source: S-out\n"
        "Terminal: T-in\n"
        "Capacity Matrix:\n"
        "\tC(A-in, A-out) = 2\n"
        "\tC(A-out, A-in) = 2\n"
        "\tC(A-out, B-in) = 2\n"
        "\tC(A-out, D-in) = 1\n"
        "\tC(A-out, E-in) = 2\n"
        "\tC(B-in, B-out) = 5\n"
        "\tC(B-out, B-in) = 5\n"
        "\tC(B-out, D-in) = 1\n"
        "\tC(B-out, T-in) = 3\n"
        "\tC(C-in, C-out) = 4\n"
        "\tC(C-out, C-in) = 4\n"
        "\tC(C-out, T-in) = 3\n"
        "\tC(D-in, D-out) = 3\n"
        "\tC(D-out, D-in) = 3\n"
        "\tC(D-out, E-in) = 2\n"
        "\tC(D-out, T-in) = 2\n"
        "\tC(E-in, E-out) = 3\n"
        "\tC(E-out, C-in) = 1\n"
        "\tC(E-out, D-in) = 2\n"
        "\tC(E-out, E-in) = 3\n"
        "\tC(E-out, T-in) = 3\n"
        "\tC(S-out, A-in) = 4\n"
        "\tC(S-out, B-in) = 4\n"
        "\tC(S-out, C-in) = 2\n";

        REQUIRE(observedNetworkStructure == expectedNetworkStructure);
    }

    // SECTION("MULTI BOUNDARY EDGE CAPACITATED") {
    //     FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromEdgeCapacitated(
    //         { "S", "A", "B", "C", "D", "E", "T" },
    //         "S",
    //         "T",

    //     );
    // }

    // SECTION("MULTI BOUNDARY VERTEX CAPACITATED") {
    //     FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromEdgeCapacitated(
    //         { "S", "A", "B", "C", "D", "E", "T" },
    //         "S",
    //         "T",

    //     );
    // }

    // SECTION("MULTI BOUNDARY EDGE AND VERTEX CAPACITATED") {
    //     FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromEdgeCapacitated(
    //         { "S", "A", "B", "C", "D", "E", "T" },
    //         "S",
    //         "T",

    //     );
    // }
}

TEST_CASE("SOLVER") {
    SECTION("GET FLOW") {

    }

    SECTION("FIND MIN CUT") {

    }

    SECTION("IS MAX FLOW") {

    }

    SECTION("AUGMENT") {

    }

    SECTION("MAXIMIZE FLOW") {

    }
}

int main() {
    return Catch::Session().run();
}