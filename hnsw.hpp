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
       private:
    // Helper to calculate Euclidean Distance
    float get_distance(const std::vector<float>& a, const std::vector<float>& b) {
        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            sum += (a[i] - b[i]) * (a[i] - b[i]);
        }
        return std::sqrt(sum);
    }

public:
    VectorItem search(const std::vector<float>& query) {
        if (entry_point == nullptr) {
            return {}; // The database is empty
        }

        HNSWNode* current_node = entry_point;
        float current_dist = get_distance(current_node->item.embeddings, query);

        // Traverse from the absolute top layer down to the bottom layer
        for (int current_layer = max_layer; current_layer >= 0; --current_layer) {
            
            bool moved_closer = true;
            
            // Greedily hop to the closest neighbor on THIS layer
            while (moved_closer) {
                moved_closer = false;
                
                for (HNSWNode* neighbor : current_node->neighbors_by_layer[current_layer]) {
                    float dist = get_distance(neighbor->item.embeddings, query);
                    
                    // If we found a closer node, move to it and keep searching
                    if (dist < current_dist) {
                        current_dist = dist;
                        current_node = neighbor;
                        moved_closer = true; 
                    }
                }
            }
            // When we can't get any closer on this layer, the loop breaks, 
            // and the outer 'for' loop drops us down to the next layer!
        }

        // Once we finish Layer 0, current_node is our nearest neighbor
        return current_node->item;
    }
        return {}; 
    }
};
