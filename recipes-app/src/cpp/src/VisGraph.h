#ifndef VISGRAPH_H_
#define VISGRAPH_H_

#include "VisEdge.h"
#include "VisNode.h"
#include <vector>

struct VisGraph {
  std::string id;
  std::vector<VisNode> nodes;
  std::vector<VisEdge> edges;

  double leniency;
  double mission_linearity;
  double map_linearity;
  double path_redundancy;

  std::string chain_str;
  std::vector<std::string> app_rules;
  size_t hash;

  static std::string getStringFromInstance(const VisGraph &instance) {
    return instance.id;
  }
};

#endif // VISGRAPH_H_
