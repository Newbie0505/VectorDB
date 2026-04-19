#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>
#include <algorithm>

#include "httplib.h"
#include "json.hpp"
#include "hnsw.hpp"

using json = nlohmann::json;

// --- STEP 1: UPGRADE THE SCHEMA ---
struct VectorItem {
    int id;
    std::string text;
    std::vector<float> embeddings;
    std::string source_file; // NEW: Metadata field
};

// --- THE MATH & KD-TREE ---
float euclidean_distance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) sum += (a[i] - b[i]) * (a[i] - b[i]);
    return std::sqrt(sum);
}

struct KDNode { VectorItem item; KDNode* left = nullptr; KDNode* right = nullptr; };

class KDTree {
public:
    KDNode* root = nullptr;
    KDNode* insertRec(KDNode* root, VectorItem item, unsigned depth) {
        if (root == nullptr) return new KDNode{item, nullptr, nullptr};
        unsigned cd = depth % item.embeddings.size();
        if (item.embeddings[cd] < root->item.embeddings[cd]) root->left = insertRec(root->left, item, depth + 1);
        else root->right = insertRec(root->right, item, depth + 1);
        return root;
    }
    void insert(const VectorItem& item) { root = insertRec(root, item, 0); }
    void searchRec(KDNode* root, const std::vector<float>& query, unsigned depth, KDNode*& best_node, float& best_dist) {
        if (root == nullptr) return;
        float dist = euclidean_distance(root->item.embeddings, query);
        if (dist < best_dist) { best_dist = dist; best_node = root; }
        unsigned cd = depth % query.size();
        KDNode* good_side = (query[cd] < root->item.embeddings[cd]) ? root->left : root->right;
        KDNode* bad_side  = (query[cd] < root->item.embeddings[cd]) ? root->right : root->left;
        searchRec(good_side, query, depth + 1, best_node, best_dist);
        if (std::abs(query[cd] - root->item.embeddings[cd]) < best_dist) {
            searchRec(bad_side, query, depth + 1, best_node, best_dist);
        }
    }
    VectorItem search(const std::vector<float>& query) {
        KDNode* best_node = nullptr; float best_dist = 999999.0f;
        searchRec(root, query, 0, best_node, best_dist);
        return best_node->item;
    }
};

// --- THE AI CLIENTS ---
std::vector<float> get_embedding(const std::string& text) {
    httplib::Client cli("http://localhost:11434");
    json request_body = {{"model", "nomic-embed-text"}, {"prompt", text}};
    auto res = cli.Post("/api/embeddings", request_body.dump(), "application/json");
    if (res && res->status == 200) return json::parse(res->body)["embedding"].get<std::vector<float>>();
    return {}; 
}

std::string ask_llm(const std::string& context, const std::string& question) {
    httplib::Client cli("http://localhost:11434");
    std::string prompt = "Using ONLY the following context, answer the question.\n\nContext: " + context + "\n\nQuestion: " + question;
    json request_body = { {"model", "llama3.2:1b"}, {"prompt", prompt}, {"stream", false} };
    auto res = cli.Post("/api/generate", request_body.dump(), "application/json");
    if (res && res->status == 200) return json::parse(res->body)["response"].get<std::string>();
    return "Error generating answer.";
}

// --- THE SERVER ---
int main() {
    HNSW db;
    httplib::Server svr;

    std::cout << "Loading Database... Please wait..." << std::endl;

    // STEP 2: PAIR TEXT WITH METADATA FILENAMES
    std::vector<std::pair<std::string, std::string>> documents = {
        {"wifi_policy.txt", "The company WiFi password is 'SuperSecret2026'."},
        {"architecture.md", "The VectorDB engine was built from scratch in C++ to make searching fast."},
        {"employee_handbook.pdf", "Employee lunch break is strictly at 12:30 PM in the main cafeteria."}
    };
    
    int id = 1;
    for (const auto& doc : documents) {
        // Insert the ID, the text, the embedding, and the filename!
        db.insert({id++, doc.second, get_embedding(doc.second), doc.first});
    }

    // NEW: Listen for POST requests from our web UI
    svr.Post("/ask", [&db](const httplib::Request& req, httplib::Response& res) {
        // Parse the question typed by the user in the browser
        json user_req = json::parse(req.body);
        std::string question = user_req["question"];
        
        // Run the RAG pipeline!
        std::vector<float> query_vector = get_embedding(question);
        VectorItem matching_doc = db.search(query_vector);
        std::string ai_answer = ask_llm(matching_doc.text, question);

        // STEP 3: SEND THE METADATA BACK TO THE UI
        json response;
        response["answer"] = ai_answer;
        response["context"] = matching_doc.text;
        response["source_file"] = matching_doc.source_file; // The UI can now display this!
        
        res.set_content(response.dump(), "application/json"); 
    });

    // NEW: Tell the C++ server to host our frontend folder
    svr.set_mount_point("/", "./");

    std::cout << "=== VectorDB Online! ===" << std::endl;
    std::cout << "Open your browser to: http://localhost:8080" << std::endl;

    svr.listen("localhost", 8080);
    return 0;
}
