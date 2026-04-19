#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>
#include <algorithm>

#include "httplib.h"
#include "json.hpp"
#include "hnsw.hpp" // This pulls in VectorItem and HNSW

using json = nlohmann::json;

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
    HNSW db; // Using your new high-performance engine
    httplib::Server svr;

    std::cout << "Loading Database with HNSW Engine... Please wait..." << std::endl;

    // Load initial documents into the HNSW graph
    std::vector<std::pair<std::string, std::string>> documents = {
        {"wifi_policy.txt", "The company WiFi password is 'SuperSecret2026'."},
        {"architecture.md", "The VectorDB engine was built from scratch in C++ using HNSW for high-speed search."},
        {"employee_handbook.pdf", "Employee lunch break is strictly at 12:30 PM in the main cafeteria."}
    };
    
    int id = 1;
    for (const auto& doc : documents) {
        db.insert({id++, doc.second, get_embedding(doc.second), doc.first});
    }

    // Handle the Ask request
    svr.Post("/ask", [&db](const httplib::Request& req, httplib::Response& res) {
        json user_req = json::parse(req.body);
        std::string question = user_req["question"];
        
        std::vector<float> query_vector = get_embedding(question);
        VectorItem matching_doc = db.search(query_vector);
        std::string ai_answer = ask_llm(matching_doc.text, question);

        json response;
        response["answer"] = ai_answer;
        response["context"] = matching_doc.text;
        response["source_file"] = matching_doc.source_file;
        
        res.set_content(response.dump(), "application/json"); 
    });

    // CRITICAL: Point this exactly to your project folder
    // Change this path to match your actual folder if "./" doesn't work
    svr.set_mount_point("/", "./");

    std::cout << "=== VectorDB Online (HNSW Mode) ===" << std::endl;
    std::cout << "Open your browser to: http://localhost:8080" << std::endl;

    svr.listen("localhost", 8080);
    return 0;
}
