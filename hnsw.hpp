#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
#include <random>

// We reuse your VectorItem struct from main.cpp
struct VectorItem {
    int id;
    std::string text;
    std::vector<float> embeddings;
    std::string source_file;
};

// --- THE NEW HNSW NODE ---
struct HNSWNode {
    VectorItem item;
    
    // Instead of left/right pointers, we have an array of layers.
    // layers[0] = list of connected neighbor pointers at the bottom layer.
    // layers[1] = list of connected neighbor pointers one layer up, etc.
    std::vector<std::vector<HNSWNode*>> neighbors_by_layer;

    // Constructor to initialize the node with a specific number of layers
    HNSWNode(VectorItem v, int num_layers) : item(v) {
        neighbors_by_layer.resize(num_layers);
    }
};

// --- THE HNSW ENGINE SKELETON ---
class HNSW {
private:
    HNSWNode* entry_point = nullptr; // The "top" of the graph
    int max_layer = -1;              // The highest layer currently in the graph
    int M = 16;                      // Max connections a node can have per layer

    // Random number generator for assigning layers
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution{0.0, 1.0};
    double level_mult = 1.0 / log(1.0 * M); // Standard HNSW probability formula

    // Function to randomly determine how high a new node goes
    int getRandomLayer() {
        double r = distribution(generator);
        if (r == 0.0) r = 0.00001; // prevent log(0)
        return (int)(-log(r) * level_mult);
    }

public:
    HNSW() {}

    // We will build these two functions next
    void insert(const VectorItem& item) {
        // TODO: The complex insertion logic
    }

    VectorItem search(const std::vector<float>& query) {
        // TODO: The layer-by-layer drop-down search
        return {}; 
    }
};
