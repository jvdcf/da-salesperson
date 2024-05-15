#ifndef DA2324_PRJ2_G163_GRAPH_HPP
#define DA2324_PRJ2_G163_GRAPH_HPP

/**
 * @file NewGraph.hpp
 * @brief A more minimalistic graph implementation inspired by the one used in the practical classes.
 * @details This implementation uses unordered maps to store vertices and edges, for faster access, compromising
 * memory usage. It also uses a more object-oriented approach, avoiding the use of pointers and dynamic memory.
 */

#include <cstdint>
#include <unordered_map>
#include <stdexcept>

// =================================================================================================

template<typename T>
class Edge {
private:
  uint64_t orig;  /// Origin vertex id
  uint64_t dest;  /// Destination vertex id
  double weight;  /// Edge weight

protected:
  double flow = 0.0;      /// Used for flow-related algorithms
  bool selected = false;  /// Used to mark the edge as artificially generated

public:
  Edge(uint64_t orig, uint64_t dest, double weight) : orig(orig), dest(dest), weight(weight) {}

  [[nodiscard]] uint64_t getOrig() const { return orig; }

  [[nodiscard]] uint64_t getDest() const { return dest; }

  [[nodiscard]] double getWeight() const { return weight; }

  [[nodiscard]] double getFlow() const { return flow; }

  [[nodiscard]] bool isSelected() const { return selected; }

  void setWeight(double w) { this->weight = w; }

  void setFlow(double f) { this->flow = f; }

  void setSelected(bool s) { this->selected = s; }
};

// =================================================================================================

template<typename T>
class Vertex {
private:
  uint64_t id;  /// Vertex id
  T info;       /// Information stored in the vertex
  std::unordered_map<uint64_t, Edge<T>> edges;  /// Adjacency list

protected:
  bool visited = false;     // Used by traversal algorithms
  bool processing = false;  // Used by DAG algorithms
  double dist = 0.0;        // Used by shortest path algorithms
  Edge<T> &path = nullptr;  // Used by shortest path algorithms
  int queueIndex = 0;       // Used by MutablePriorityQueue

  Edge<T> &addEdge(Vertex<T> &dest, double weight) {
    Edge<T> e = Edge<T>(this->info, dest.getInfo(), weight);
    edges[dest.getInfo()] = e;
    return edges[dest.getInfo()];
  }

  void removeEdge(Vertex<T> &dest) {
    edges.erase(dest.getInfo());
  }

public:
  explicit Vertex(T info, uint64_t id) : info(info), id(id) {}

  [[nodiscard]] T getInfo() const { return info; }

  [[nodiscard]] uint64_t getId() const { return id; }

  [[nodiscard]] std::unordered_map<uint64_t, Edge<T>> &getAdj() { return edges; }

  [[nodiscard]] bool isVisited() const { return visited; }

  [[nodiscard]] bool isProcessing() const { return processing; }

  [[nodiscard]] double getDist() const { return dist; }

  [[nodiscard]] Edge<T> &getPath() const { return path; }

  void setVisited(bool v) { this->visited = v; }

  void setProcessing(bool p) { this->processing = p; }

  void setDist(double d) { this->dist = d; }

  void setPath(Edge<T> &p) { this->path = p; }
};

// =================================================================================================

template<typename T>
class Graph {
private:
  std::unordered_map<uint64_t, Vertex<T>> vertexSet;
public:
  Graph() = default;

  explicit Graph(unsigned int numVertex) {
    vertexSet.reserve(numVertex);
  }

  [[nodiscard]] std::unordered_map<uint64_t, Vertex<T>> &getVertexSet() { return vertexSet; }

  Vertex<T> &addVertex(T info, uint64_t id) {
    vertexSet[id] = Vertex<T>(info, id);
    return vertexSet[id];
  }

  void removeVertex(uint64_t id) {
    vertexSet.erase(id);
  }

  Edge<T> &addEdge(Vertex<T> &orig, Vertex<T> &dest, double weight) {
    return orig.addEdge(dest, weight);
  }

  void removeEdge(Vertex<T> &orig, Vertex<T> &dest) {
    orig.removeEdge(dest);
  }

  void addBidirectionalEdge(Vertex<T> &orig, Vertex<T> &dest, double weight) {
    addEdge(orig, dest, weight);
    addEdge(dest, orig, weight);
  }

  [[nodiscard]] Vertex<T> &findVertex(uint64_t id) {
    try { return vertexSet.at(id); }
    catch (std::out_of_range &e) { return nullptr; }
  }

  [[nodiscard]] Edge<T> &findEdge(uint64_t orig, uint64_t dest) {
    try { return vertexSet.at(orig).getAdj().at(dest); }
    catch (std::out_of_range &e) { return nullptr; }
  }

  int getNumVertex() {
    return vertexSet.size();
  }
};

#endif //DA2324_PRJ2_G163_GRAPH_HPP
