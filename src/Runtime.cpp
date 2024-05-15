#include "Runtime.h"
#include "Utils.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

Runtime::Runtime(Data *d) { this->data = d; }

[[noreturn]] void Runtime::run() {
  std::cout << "Welcome to SMS (Shipping Management System).\n"
            << "Type 'help' to learn more.\n";

  std::string input, buf;

  while (true) {
    input.clear();

    std::cout << "> ";
    std::getline(std::cin, input);
    if (input.empty())
      continue;
    std::istringstream iss(input);
    processArgs(iss);
  }
}

void Runtime::printHelp() {
  auto keyword = Color(166, 209, 137).foreground();
  auto comment = Color(249, 226, 175).foreground();
  std::cout << "Available commands:\n"
            << keyword << "  quit\n"
            << comment << "      Quits this program.\n"
            << keyword << "  help\n"
            << comment << "      Prints this help.\n"
            << keyword << "  count\n"
            << comment << "      Prints the number of vertices and edges.\n"
            << keyword << "  backtracking\n"
            << comment << "      Resolves the TSP problem using backtracking.\n"
            << comment << "      If the graph is not complete, this command will generate the remaining edges using the coordinates inside nodes.csv.\n"
            << keyword << "  triangular\n"
            << comment << "      Generates an approximation of the TSP problem using the triangular heuristic.\n"
            << comment << "      If the graph is not complete, this command will generate the remaining edges using the coordinates inside nodes.csv.\n"
            << keyword << "  heuristic\n"
            << comment << "      Generates an approximation of the TSP problem using // TODO.\n"
            << comment << "      If the graph is not complete, this command will generate the remaining edges using the coordinates inside nodes.csv.\n"
            << keyword << "  disconnected <vertex-id>\n"
            << comment << "      Generates an approximation of the TSP problem using // TODO.\n"
            << comment << "      This command will not assume any edge not given by the .csv files.\n"
            << Color::clear() << std::endl;
}

void Runtime::handleQuit() {
  info("Quitting...");
  exit(0);
}

void Runtime::handleCount() {
  auto &g = data->getGraph();
  unsigned int edgeCount = 0;
  for (auto v : g.getVertexSet()) {
    edgeCount += v.second.getAdj().size();
  }
  std::cout << "Number of vertices: " << g.getVertexSet().size() << std::endl;
  std::cout << "Number of edges: " << edgeCount << std::endl;
}

void Runtime::handleBacktracking() {
  std::cout << data->backtracking() << std::endl;
}

void Runtime::handleTriangular() {
  std::cout << data->triangular() << std::endl;
}

void Runtime::handleHeuristic() {
  std::cout << data->heuristic() << std::endl;
}

void Runtime::handleDisconnected(Command &cmd) {
  unsigned vertexId = cmd.args.at(0).getInt().value();
  Graph<Info> &g = data->getGraph();
  if (!g.hasVertex(vertexId)) {
    error("Vertex " + std::to_string(vertexId) + " does not exist.");
    return;
  }
  auto result = data->disconnected(vertexId);
  if (!result.has_value()) {
    info("No hamiltonian path starting at vertex " + std::to_string(vertexId) + " was found.");
  } else {
    std::cout << result.value() << std::endl;
  }
}

void Runtime::processArgs(std::istream &args) {
  constexpr auto cmd_parser = parse_cmd();
  auto pos = args.tellg();
  auto cmd_res = cmd_parser.to_fn()(args);
  if (!cmd_res.has_val) {
    args.clear();
    args.seekg(pos);
    std::string arg_contents(std::istreambuf_iterator<char>(args), {});
    return error("The command '" + arg_contents +
                 "' is invalid. Type 'help' to know more.");
  }
  auto cmd = cmd_res.ok;

  std::string rest(std::istreambuf_iterator<char>(args), {});
  if (!rest.empty())
    warning("Trailing output: '" + rest + "'.");

  clock.start();
  switch (cmd.command) {
  case Command::Help:
    return printHelp();
  case Command::Quit:
    return handleQuit();
  case Command::Count:
    return handleCount();
  case Command::Backtracking:
    handleBacktracking();
    break;
  case Command::Triangular:
    handleTriangular();
    break;
  case Command::Heuristic:
    handleHeuristic();
    break;
  case Command::Disconnected:
    handleDisconnected(cmd);
    break;
  default:
    info("Type 'help' to see the available commands.");
    return;
  }
  clock.stop();
  std::cout << Color(183, 189, 248).foreground() << "Time elapsed: " << clock << Color::clear() << std::endl;
}
