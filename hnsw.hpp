#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
#include <random>
#include <algorithm>

// 1. The Vector Data Structure
struct VectorItem {
    int id;
    std::string text;
    std::vector<float> embeddings;
    std::string source_file;
};

// 2. The HNSW Node Structure
struct HNSWNode {
    VectorItem item;
    std::vector<std::vector<HNSWNode*>> neighbors_by_layer;

    HNSWNode(VectorItem v, int num_layers) : item(v) {
        neighbors_by_layer.resize(num_layers);
    }
};

// 3. The HNSW Engine Core
class HNSW {
private:
    HNSWNode* entry_point = nullptr; 
    int max_layer = -1;              
    int M = 16;                      

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution{0.0, 1.0};
    double level_mult = 1.0 / log(1.0 * M);

    int getRandomLayer() {
        double r = distribution(generator);
        if (r == 0.0) r = 0.00001; 
        return (int)(-log(r) * level_mult);
    }

    float get_distance(const std::vector<float>& a, const std::vector<float>& b) {
        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) sum += (a[i] - b[i]) * (a[i] - b[i]);
        return std::sqrt(sum);
    }

public:
    HNSW() {}

    void insert(const VectorItem& item) {
        int node_layer = getRandomLayer();
        HNSWNode* new_node = new HNSWNode(item, node_layer + 1);

        if (entry_point == nullptr) {
            entry_point = new_node;
            max_layer = node_layer;
            return;
        }

        HNSWNode* current_node = entry_point;
        float current_dist = get_distance(current_node->item.embeddings, item.embeddings);

        // Phase 1: The Ghost Drop
        for (int current_layer = max_layer; current_layer > node_layer; --current_layer) {
            bool moved_closer = true;
            while (moved_closer) {
                moved_closer = false;
                for (HNSWNode* neighbor : current_node->neighbors_by_layer[current_layer]) {
                    float dist = get_distance(neighbor->item.embeddings, item.embeddings);
                    if (dist < current_dist) {
                        current_dist = dist;
                        current_node = neighbor;
                        moved_closer = true;
                    }
                }
            }
        }

        // Phase 2: Weaving the Web
        int start_layer = std::min(max_layer, node_layer);
        for (int current_layer = start_layer; current_layer >= 0; --current_layer) {
            bool moved_closer = true;
            while (moved_closer) {
                moved_closer = false;
                for (HNSWNode* neighbor : current_node->neighbors_by_layer[current_layer]) {
                    float dist = get_distance(neighbor->item.embeddings, item.embeddings);
                    if (dist < current_dist) {
                        current_dist = dist;
                        current_node = neighbor;
                        moved_closer = true;
                    }
                }
            }

            new_node->neighbors_by_layer[current_layer].push_back(current_node);
            current_node->neighbors_by_layer[current_layer].push_back(new_node);

            int connections_made = 1;
            for (HNSWNode* neighbor_of_neighbor : current_node->neighbors_by_layer[current_layer]) {
                if (connections_made >= M) break; 
                if (neighbor_of_neighbor != new_node) {
                    new_node->neighbors_by_layer[current_layer].push_back(neighbor_of_neighbor);
                    neighbor_of_neighbor->neighbors_by_layer[current_layer].push_back(new_node);
                    connections_made++;
                }
            }
        }

        // Phase 3: Update the Crown
        if (node_layer > max_layer) {
            max_layer = node_layer;
            entry_point = new_node;
        }
    }

    VectorItem search(const std::vector<float>& query) {
        if (entry_point == nullptr) return {};

        HNSWNode* current_node = entry_point;
        float current_dist = get_distance(current_node->item.embeddings, query);

        for (int current_layer = max_layer; current_layer >= 0; --current_layer) {
            bool moved_closer = true;
            while (moved_closer) {
                moved_closer = false;
                for (HNSWNode* neighbor : current_node->neighbors_by_layer[current_layer]) {
                    float dist = get_distance(neighbor->item.embeddings, query);
                    if (dist < current_dist) {
                        current_dist = dist;
                        current_node = neighbor;
                        moved_closer = true; 
                    }
                }
            }
        }
        return current_node->item;
    }
};
