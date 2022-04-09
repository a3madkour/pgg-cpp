#include "Node.h"

Node::Node() { rhs_index = -1; }
Node::Node(std::string r_id, std::unordered_set<std::string> r_set)
    : rule_id(r_id), rule_set(r_set) {}
Node::Node(std::string r_id, std::unordered_set<std::string> r_set, int rhs_i)
    : rule_id(r_id), rule_set(r_set), rhs_index(rhs_i) {}

std::string Node::toJSON(
    std::unordered_map<std::unordered_set<std::string>, int, RuleSSetHash>
        &rule_index_map) {
  std::stringstream ss;
  ss << "{ \n";
  ss << "\"rule_id\":\"" << rule_id << "\",\n";
  int index = rule_index_map[rule_set];
  ss << "\"rule_set_index\":\"" << index << "\",\n";
  ss << "\"rhs_i\":\"" << rhs_index << "\"\n";
  ss << "} \n";
  return ss.str();
}
