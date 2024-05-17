#include <cmath>
#include <iostream>
#include <ostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <random>
#include "Utils.h"
#include "../lib/MutablePriorityQueue.h"

// Colors class ================================================================

[[noreturn]] void panic(std::string s) {
  auto c = Color(255,100,100);
  std::cerr << c.foreground() << "[CRITICAL ERR] " << c.clear() << s << std::endl;
  std::exit(1);
}

void error(std::string s) {
  auto c = Color(255,100,0);
  std::cerr << c.foreground() << "[ERROR] " << c.clear() << s << std::endl;
}

void info(std::string s) {
  auto c = Color(0,235,235);
  std::cerr << c.foreground() << "[INFO] " << c.clear() << s << std::endl;
}

void warning(std::string s) {
  auto c = Color(255,255,15);
  std::cerr << c.foreground() <<"[WARNING] " << c.clear() << s << std::endl;
}

// Clock class =================================================================

Clock::Clock() {
  this->start_time = std::chrono::high_resolution_clock::now();
  this->end_time = std::chrono::high_resolution_clock::now();
}

void Clock::start() {
  this->start_time = std::chrono::high_resolution_clock::now();
}

void Clock::stop() {
  this->end_time = std::chrono::high_resolution_clock::now();
}

double Clock::getTime() const {
  auto duration = this->end_time - this->start_time;
  long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  return (double)microseconds / 1000.0;
}

// Terminal Utils ==============================================================

unsigned Utils::countLines(const std::string& path) {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  return std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
}

void Utils::printLoading(unsigned current, unsigned total, const std::string& message) {
  float percentage = std::round((float)current / total * 100);
  if (current * 10000 / total != (current - 1) * 10000 / total) {
    std::cout << message << ": "
              << current / 1000 << "K / " << total / 1000
              << "K (" << percentage << "%)     \r";
    std::cout.flush();
  }
}

void Utils::clearLine() {
  std::cout << "                                                                          \r";
  std::cout.flush();
}

// Project Utils ==============================================================

double Utils::convertToRadians(double angle) {
  return angle * M_PI / 180;
}

double Utils::haversineDistance(double lat1, double lon1, double lat2, double lon2) {
  lat1 = convertToRadians(lat1);
  lon1 = convertToRadians(lon1);
  lat2 = convertToRadians(lat2);
  lon2 = convertToRadians(lon2);

  double delta_lat = lat2 - lat1;
  double delta_lon = lon2 - lon1;

  double aux = pow(sin(delta_lat/2), 2) + cos(lat1) * cos(lat2) * pow(sin(delta_lon/2), 2);
  double c = 2.0 * atan2(sqrt(aux), sqrt(1-aux));
  double const R = 6371000.0; // Earth radius in meters

  return R * c;
}

void Utils::prim(Graph<Info> * g) {

  for (auto& [id, v] : g->getVertexSet()){
    v.setVisited(false);
    v.setDist(INF);
    v.setPath(UINT64_MAX);
  }

  Vertex<Info>& vertex= g->findVertex(0); //start with vertex 0
  vertex.setDist(0);

  MutablePriorityQueue<Vertex<Info>> q;
  q.insert(&vertex);

  while (!q.empty()){

    Vertex<Info> * v = q.extractMin();
    v->setVisited(true);

    for (auto& [destId, e] : v->getAdj()){
      Vertex<Info>& u = g->findVertex(destId);

      if (!u.isVisited() && e.getWeight() < u.getDist()) {

        if (u.getDist() == INF) q.insert(&u);
        else q.decreaseKey(&u);

        u.setDist(e.getWeight());
        u.setPath(v->getId());

      }

    }

  }

}

std::vector<uint64_t> Utils::MSTdfs(Graph<Info> * g){
  std::vector<uint64_t> res;
  for (auto& [id, v]  : g->getVertexSet())
    v.setVisited(false);

  Vertex<Info>& first = g->findVertex(0);

  MSTdfsVisit(first, res, g);

  for (auto& [id, v]  : g->getVertexSet())
    if (!v.isVisited())
      MSTdfsVisit(v, res, g);

  return res;
}

void Utils::MSTdfsVisit(Vertex<Info>& v, std::vector<uint64_t> & res, Graph<Info> * g) {
  v.setVisited(true);

  res.push_back(v.getId());

  for (auto& [destId, e] : v.getAdj()) {
    Vertex<Info>& u = g->findVertex(destId);

    if (u.getPath()){
      if (u.getPath() == v.getId()) {
        if (!u.isVisited()) {
          MSTdfsVisit(u, res, g);
        }
      }
    }
  }
}

double Utils::weight(uint64_t v, uint64_t u, Graph<Info> * g) {
  Edge<Info>* e =g->findEdge(v, u);

  if (e) return e->getWeight();
  return g->findVertex(v).getInfo().distance(g->findVertex(u).getInfo());
}


bool Utils::isConnected(Graph<Info> *g) {

  for (auto& [id, v] : g->getVertexSet())
    v.setVisited(false);

  Vertex<Info> first = g->getVertexSet().at(0);

  dfs(&first, g);

  for (auto& [id, v] : g->getVertexSet())
    if (!v.isVisited())
      return false;

  return true;
}

void Utils::dfs(Vertex<Info> *v, Graph<Info> * g) {
  v->setVisited(true);

  for (auto& [destId, e] : v->getAdj()) {
    Vertex<Info>& u = g->findVertex(destId);
    if (!u.isVisited())
      dfs(&u, g);
  }
}


