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

    SECTION("MULTI BOUNDARY EDGE CAPACITATED") {
        FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromMultiBoundaryEdgeCapacitated(
            { "S1", "S2", "A", "B", "C", "T1", "T2" },
            { "S1", "S2" },
            { "T1", "T2" },
            {
                Edge("S1", "A", 2),
                Edge("S1", "B", 2),
                Edge("S1", "C", 4),
                Edge("S2", "C", 2),
                Edge("S2", "T2", 3),
                Edge("A", "T1", 3),
                Edge("B", "T1", 7),
                Edge("C", "T2", 4),
                Edge("C", "B", 5)
            }
        );

        std::string observedNetworkStructure = network.toString();
        std::string expectedNetworkStructure =
        "Nodes: $S, $T, A, B, C, S1, S2, T1, T2\n"
        "Source: $S\n"
        "Terminal: $T\n"
        "Capacity Matrix:\n"
        "\tC($S, S1) = 32\n"
        "\tC($S, S2) = 32\n"
        "\tC(A, T1) = 3\n"
        "\tC(B, T1) = 7\n"
        "\tC(C, B) = 5\n"
        "\tC(C, T2) = 4\n"
        "\tC(S1, A) = 2\n"
        "\tC(S1, B) = 2\n"
        "\tC(S1, C) = 4\n"
        "\tC(S2, C) = 2\n"
        "\tC(S2, T2) = 3\n"
        "\tC(T1, $T) = 32\n"
        "\tC(T2, $T) = 32\n";

        REQUIRE(observedNetworkStructure == expectedNetworkStructure);
    }

    SECTION("MULTI BOUNDARY VERTEX CAPACITATED") {
        FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromMultiBoundaryVertexCapacitated(
            { "S1", "S2", "S3", "A", "B", "C", "D", "T1", "T2" },
            { "S1", "S2", "S3" },
            { "T1", "T2" },
            {
                { "S1", "A" },
                { "S2", "C" },
                { "S3", "C" },
                { "A", "B" },
                { "B", "D" },
                { "B", "T1" },
                { "C", "B" },
                { "C", "D" },
                { "D", "T2" },
            },
            {
                { "A", 2 },
                { "B", 9 },
                { "C", 11 },
                { "D", 3 }
            }
        );

        std::string observedNetworkStructure = network.toString();
        std::string expectedNetworkStructure =
        "Nodes: $S-out, $T-in, A-in, A-out, B-in, B-out, C-in, C-out, D-in, D-out, S1-in, S1-out, S2-in, S2-out, S3-in, S3-out, T1-in, T1-out, T2-in, T2-out\n"
        "Source: $S-out\n"
        "Terminal: $T-in\n"
        "Capacity Matrix:\n"
        "\tC($S-out, S1-in) = 25\n"
        "\tC($S-out, S2-in) = 25\n"
        "\tC($S-out, S3-in) = 25\n"
        "\tC(A-in, A-out) = 2\n"
        "\tC(A-out, A-in) = 2\n"
        "\tC(A-out, B-in) = 25\n"
        "\tC(B-in, B-out) = 9\n"
        "\tC(B-out, B-in) = 9\n"
        "\tC(B-out, D-in) = 25\n"
        "\tC(B-out, T1-in) = 25\n"
        "\tC(C-in, C-out) = 11\n"
        "\tC(C-out, B-in) = 25\n"
        "\tC(C-out, C-in) = 11\n"
        "\tC(C-out, D-in) = 25\n"
        "\tC(D-in, D-out) = 3\n"
        "\tC(D-out, D-in) = 3\n"
        "\tC(D-out, T2-in) = 25\n"
        "\tC(S1-in, S1-out) = 25\n"
        "\tC(S1-out, A-in) = 25\n"
        "\tC(S1-out, S1-in) = 25\n"
        "\tC(S2-in, S2-out) = 25\n"
        "\tC(S2-out, C-in) = 25\n"
        "\tC(S2-out, S2-in) = 25\n"
        "\tC(S3-in, S3-out) = 25\n"
        "\tC(S3-out, C-in) = 25\n"
        "\tC(S3-out, S3-in) = 25\n"
        "\tC(T1-in, T1-out) = 25\n"
        "\tC(T1-out, $T-in) = 25\n"
        "\tC(T1-out, T1-in) = 25\n"
        "\tC(T2-in, T2-out) = 25\n"
        "\tC(T2-out, $T-in) = 25\n"
        "\tC(T2-out, T2-in) = 25\n";

        REQUIRE(observedNetworkStructure == expectedNetworkStructure);
    }

    SECTION("MULTI BOUNDARY EDGE AND VERTEX CAPACITATED") {
        FlowCapacitatedNetwork network = FlowCapacitatedNetwork::fromMultiBoundaryEdgeAndVertexCapacitated(
            { "S1", "S2", "A", "B", "C", "D", "T1", "T2" },
            { "S1", "S2" },
            { "T1", "T2" },
            {
                Edge("S1", "A", 5),
                Edge("S2", "C", 10),
                Edge("A", "B", 4),
                Edge("A", "T1", 4),
                Edge("B", "D", 5),
                Edge("C", "B", 8),
                Edge("C", "D", 5),
                Edge("D", "T2", 9),
            },
            {
                { "A", 5 },
                { "B", 10 },
                { "C", 12 },
                { "D", 8 }
            }
        );

        std::string observedNetworkStructure = network.toString();
        std::string expectedNetworkStructure =
        "Nodes: $S-out, $T-in, A-in, A-out, B-in, B-out, C-in, C-out, D-in, D-out, S1-in, S1-out, S2-in, S2-out, T1-in, T1-out, T2-in, T2-out\n"
        "Source: $S-out\n"
        "Terminal: $T-in\n"
        "Capacity Matrix:\n"
        "\tC($S-out, S1-in) = 85\n"
        "\tC($S-out, S2-in) = 85\n"
        "\tC(A-in, A-out) = 5\n"
        "\tC(A-out, A-in) = 5\n"
        "\tC(A-out, B-in) = 4\n"
        "\tC(A-out, T1-in) = 4\n"
        "\tC(B-in, B-out) = 10\n"
        "\tC(B-out, B-in) = 10\n"
        "\tC(B-out, D-in) = 5\n"
        "\tC(C-in, C-out) = 12\n"
        "\tC(C-out, B-in) = 8\n"
        "\tC(C-out, C-in) = 12\n"
        "\tC(C-out, D-in) = 5\n"
        "\tC(D-in, D-out) = 8\n"
        "\tC(D-out, D-in) = 8\n"
        "\tC(D-out, T2-in) = 9\n"
        "\tC(S1-in, S1-out) = 85\n"
        "\tC(S1-out, A-in) = 5\n"
        "\tC(S1-out, S1-in) = 85\n"
        "\tC(S2-in, S2-out) = 85\n"
        "\tC(S2-out, C-in) = 10\n"
        "\tC(S2-out, S2-in) = 85\n"
        "\tC(T1-in, T1-out) = 85\n"
        "\tC(T1-out, $T-in) = 85\n"
        "\tC(T1-out, T1-in) = 85\n"
        "\tC(T2-in, T2-out) = 85\n"
        "\tC(T2-out, $T-in) = 85\n"
        "\tC(T2-out, T2-in) = 85\n";

        REQUIRE(observedNetworkStructure == expectedNetworkStructure);
    }
}

TEST_CASE("SOLVER") {
    SECTION("MIN CUT AND MAX FLOW") {
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
                Edge("D", "T", 3),
            }
        );

        network.maximizeFlow();

        REQUIRE(network.getFlow() == 5);
        
        auto [sPartition, tPartition] = network.findMinCut();

        REQUIRE(sPartition == std::unordered_set<std::string>{ "S", "C" });
        REQUIRE(tPartition == std::unordered_set<std::string>{ "A", "B", "D", "T" });
    }
}

int main() {
    return Catch::Session().run();
}