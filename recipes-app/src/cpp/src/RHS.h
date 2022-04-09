#ifndef RHS_H_
#define RHS_H_
#include "Graph.h"
#include <iostream>
struct RHS {
  Graph graph;
  double prob;
  RHS();
  RHS(Graph g, double p);
  friend std::ostream &operator<<(std::ostream &os, const RHS &rhs);
};
#endif // RHS_H_
