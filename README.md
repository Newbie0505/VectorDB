# 🧠 VectorDB CORE

![C++](https://img.shields.io/badge/C++-17-blue.svg?logo=c%2B%2B)
![Vanilla JS](https://img.shields.io/badge/Frontend-Vanilla_JS-yellow.svg?logo=javascript)
![Local AI](https://img.shields.io/badge/AI-Ollama_Local-orange.svg)

A high-performance, fully local Vector Database and Retrieval-Augmented Generation (RAG) pipeline built from scratch in C++. 

This project demonstrates the foundational architecture of modern AI semantic search engines (like Pinecone or Weaviate) without relying on external cloud APIs or heavy web frameworks. It runs entirely on local hardware, prioritizing privacy and speed.

## ✨ Features

- **Custom Vector Engine:** Built completely from scratch in C++ without external database dependencies.
- **Logarithmic Search (KD-Tree):** Implements a K-Dimensional Tree for optimized spatial partitioning and nearest-neighbor search, efficiently handling high-dimensional vectors.
- **Local Neural Embeddings:** Integrates with `nomic-embed-text` via Ollama to map semantic meaning into 768-dimensional space.
- **Fully Local RAG Pipeline:** Uses `llama3.2:1b` to generate contextualized, grounded answers based *only* on retrieved database segments.
- **Dependency-Free Frontend:** A sleek, zero-latency Vanilla JavaScript and CSS UI featuring glassmorphism, animated states, and responsive design.
- **Embedded Web Server:** Uses a lightweight, single-header C++ HTTP server to bridge the native backend with the web frontend.

## 📐 System Architecture

1. **Ingestion:** Text documents are embedded via Ollama (768-D) and inserted into the C++ KD-Tree.
2. **Querying:** User inputs natural language via the Web UI.
3. **Retrieval:** The query is embedded, and the KD-Tree performs a mathematically optimized Euclidean distance search to find the closest semantic node.
4. **Generation:** The retrieved context + user query is passed to the local LLM.
5. **Delivery:** The grounded response and the exact database sources are streamed back to the UI.

## 🛠️ Tech Stack
* **Backend:** C++17
* **Search Algorithm:** Custom KD-Tree implementation
* **AI/LLM Provider:** Ollama (Local)
* **Frontend:** HTML5, CSS3, Vanilla JS
* **Networking:** `cpp-httplib` (HTTP), `nlohmann/json` (Data parsing)
