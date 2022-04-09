#ifndef NODE_H_
#define NODE_H_

#include "Rule.h"
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/vector.hpp>
#include <unordered_set>

typedef std::pair<std::unordered_map<std::string, int>, int> RuleApplication;
typedef std::unordered_map<std::string, RuleApplication> RuleAppMap;
typedef std::unordered_map<std::unordered_set<std::string>, RuleApplication,
                           RuleSSetHash>
    RuleSetMap;
struct Node {

  std::string rule_id;
  std::unordered_set<std::string> rule_set;
  int rhs_index;
  std::string toJSON(
      std::unordered_map<std::unordered_set<std::string>, int, RuleSSetHash> &);
  Node();
  Node(std::string r_id, std::unordered_set<std::string> r_set);
  Node(std::string r_id, std::unordered_set<std::string> r_set, int rhs_i);
};
#endif // NODE_H_
