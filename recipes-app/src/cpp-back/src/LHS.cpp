#include "LHS.h"

LHS::LHS(Graph g) : graph(std::move(g)) {}
LHS::LHS() {}

std::ostream &operator<<(std::ostream &os, const LHS &lhs) {
  os << lhs.graph;
  return os;
}
