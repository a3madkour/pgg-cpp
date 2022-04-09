#ifndef LHS_H_
#define LHS_H_

#include "Graph.h"
struct LHS {
  Graph graph;
  LHS();
  LHS(Graph g);
  friend std::ostream &operator<<(std::ostream &os, const LHS &lhs);
};
#endif // LHS_H_
