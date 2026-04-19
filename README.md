Demo link: https://github.com/Newbie0505/VectorDB/blob/main/Live%20demo.%20.gif
<div align="center">

<h1>⚡ VectorDB CORE</h1>

<p>
  <strong>A high-performance, fully local Vector Database & RAG pipeline — built from scratch in C++.</strong><br/>
  No cloud. No frameworks. No compromise.
</p>

<p>
  <img src="https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
  <img src="https://img.shields.io/badge/JavaScript-Vanilla-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black" />
  <img src="https://img.shields.io/badge/AI-Ollama_Local-FF6600?style=for-the-badge" />
  <img src="https://img.shields.io/badge/License-MIT-green?style=for-the-badge" />
</p>

<p>
  <a href="#-overview">Overview</a> •
  <a href="#-features">Features</a> •
  <a href="#-architecture">Architecture</a> •
  <a href="#-getting-started">Getting Started</a> •
  <a href="#-tech-stack">Tech Stack</a> •
  <a href="#-usage">Usage</a> •
  <a href="#-roadmap">Roadmap</a>
</p>

<br/>

> Demonstrating the foundational architecture behind modern semantic search engines like **Pinecone** and **Weaviate** — entirely on local hardware, with zero external dependencies.

</div>

---

## 📌 Overview

**VectorDB CORE** is a production-inspired Vector Database and Retrieval-Augmented Generation (RAG) engine written entirely in C++17. It combines a custom-built KD-Tree spatial index with local neural embeddings via Ollama to deliver fast, private, and grounded semantic search — all running offline on your own machine.

This project is a deep dive into the internals of AI-powered search systems: how vectors are stored, indexed, and retrieved, and how retrieved context can be used to ground LLM responses to facts — not hallucinations.

---

## ✨ Features

| Feature | Description |
|---|---|
| 🌲 **Custom KD-Tree Engine** | Hand-built K-Dimensional Tree for logarithmic nearest-neighbor search across high-dimensional vector spaces |
| 🧠 **Local Neural Embeddings** | Integrates with `nomic-embed-text` via Ollama to produce 768-D semantic vectors |
| 🔍 **Optimized Vector Search** | Euclidean distance search with spatial partitioning — no brute-force scanning |
| 🤖 **Fully Local RAG Pipeline** | `llama3.2:1b` generates answers grounded *only* in retrieved database segments |
| 🌐 **Embedded HTTP Server** | Lightweight C++ web server bridges the native backend to the web frontend |
| 🎨 **Dependency-Free Frontend** | Zero-framework Vanilla JS/CSS UI with glassmorphism, animated states, and source attribution |
| 🔒 **100% Local & Private** | No API keys. No telemetry. No cloud. Everything runs on your hardware. |

---

## 📐 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        Web Frontend                         │
│              (Vanilla JS · CSS Glassmorphism)               │
└──────────────────────────┬──────────────────────────────────┘
                           │  HTTP (cpp-httplib)
┌──────────────────────────▼──────────────────────────────────┐
│                    C++ Backend Server                        │
│                                                             │
│   ┌─────────────┐    ┌──────────────┐    ┌──────────────┐  │
│   │  Ingestion  │    │  KD-Tree     │    │  RAG Engine  │  │
│   │  Pipeline   │───▶│  Vector DB   │───▶│  (llama3.2)  │  │
│   └──────┬──────┘    └──────────────┘    └──────────────┘  │
│          │                                                   │
└──────────┼───────────────────────────────────────────────────┘
           │  Ollama API (Local)
┌──────────▼───────────────────────────────────────────────────┐
│              nomic-embed-text  (768-D Embeddings)            │
└──────────────────────────────────────────────────────────────┘
```

### 🔄 Pipeline Flow

```
1. INGEST    →  Document text is embedded via Ollama → 768-D float vector
2. INDEX     →  Vector is inserted into the C++ KD-Tree with spatial partitioning
3. QUERY     →  User submits natural language via Web UI
4. RETRIEVE  →  Query is embedded → KD-Tree performs Euclidean nearest-neighbor search
5. GENERATE  →  Top context + query is sent to llama3.2:1b for grounded response
6. DELIVER   →  Response + source segments are streamed back to the UI
```

---

## 🛠️ Tech Stack

| Layer | Technology |
|---|---|
| **Backend** | C++17 |
| **Search Algorithm** | Custom KD-Tree (from scratch) |
| **Embeddings** | `nomic-embed-text` via Ollama (768-D) |
| **LLM** | `llama3.2:1b` via Ollama (local) |
| **HTTP Server** | [`cpp-httplib`](https://github.com/yhirose/cpp-httplib) |
| **JSON Parsing** | [`nlohmann/json`](https://github.com/nlohmann/json) |
| **Frontend** | HTML5, CSS3, Vanilla JavaScript |

---

## 🚀 Getting Started

### Prerequisites

Ensure the following are installed on your system:

- **C++17** compatible compiler (`g++ >= 9` or `clang++ >= 10`)
- **[Ollama](https://ollama.ai)** running locally
- `make` (or equivalent build tool)

### 1. Pull Required Models

```bash
ollama pull nomic-embed-text
ollama pull llama3.2:1b
```

### 2. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/vectordb-core.git
cd vectordb-core
```

### 3. Build the Project

```bash
make build
# or manually:
g++ -std=c++17 -O2 -o vectordb main.cpp -lpthread
```

### 4. Run the Server

```bash
./vectordb
# Server starts at http://localhost:8080
```

### 5. Open the UI

Navigate to **`http://localhost:8080`** in your browser. Ingest documents, run semantic queries, and get grounded AI responses — entirely offline.

---

## 💡 Usage

### Ingesting a Document

Via the Web UI or directly via the API:

```bash
curl -X POST http://localhost:8080/ingest \
  -H "Content-Type: application/json" \
  -d '{"text": "Your document content here..."}'
```

### Querying the Database

```bash
curl -X POST http://localhost:8080/query \
  -H "Content-Type: application/json" \
  -d '{"query": "What does the document say about X?"}'
```

### Response Format

```json
{
  "answer": "Based on the retrieved context...",
  "sources": [
    { "segment": "...retrieved passage...", "distance": 0.042 }
  ]
}
```

---

## 📁 Project Structure

```
vectordb-core/
├── src/
│   ├── main.cpp          # Entry point & HTTP server setup
│   ├── kdtree.hpp        # Custom KD-Tree implementation
│   ├── embedder.cpp      # Ollama embedding integration
│   └── rag.cpp           # Retrieval & generation pipeline
├── frontend/
│   ├── index.html        # Web UI
│   ├── style.css         # Glassmorphism styling
│   └── app.js            # Vanilla JS client
├── include/
│   ├── httplib.h         # cpp-httplib (single header)
│   └── json.hpp          # nlohmann/json (single header)
├── Makefile
└── README.md
```

---

## 🗺️ Roadmap

- [x] Custom KD-Tree vector indexing
- [x] Ollama embedding integration (768-D)
- [x] Local RAG pipeline with `llama3.2:1b`
- [x] Embedded HTTP server + Vanilla JS UI
- [ ] Approximate Nearest Neighbor (ANN) with HNSW
- [ ] Multi-document ingestion & batch processing
- [ ] Persistent vector storage (binary serialization)
- [ ] Cosine similarity support alongside Euclidean
- [ ] Docker containerization
- [ ] Benchmark suite vs. Pinecone / Weaviate

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/ann-search`)
3. Commit your changes (`git commit -m 'Add HNSW approximate search'`)
4. Push to the branch (`git push origin feature/ann-search`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

<div align="center">

Built with 🧠 and ⚡ by [YOUR_NAME](https://github.com/YOUR_USERNAME)

*If this project helped you understand vector databases, consider giving it a ⭐*

</div>
