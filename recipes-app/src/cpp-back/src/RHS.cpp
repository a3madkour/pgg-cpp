#include "RHS.h"

RHS::RHS() : prob(1) {}
RHS::RHS(Graph g, double p) : graph(std::move(g)), prob(p) {}
// std::ostream &operator<<(std::ostream &os, const RHS &rhs) {
//   os << "graph" << std::endl;
//   os << rhs.graph;
//   os << "prob: " << rhs.prob;
//   return os;
// }
