#ifndef RULE_H_
#define RULE_H_

#include "LHS.h"
#include "RHS.h"
#include <cstddef>
#include <string>
#include <vector>

struct Rule {
  std::string name;
  LHS lhs;
  std::vector<RHS> rhss;
  double prob;
  std::hash<std::string> hasher;
  Rule();
  Rule(std::string name, LHS lhs, std::vector<RHS> rhss, double prob);
  // friend std::ostream &operator<<(std::ostream &os, const Rule &rule);
  friend bool operator==(const Rule &left, const Rule &right);
  std::size_t hash() const;
};
struct RuleHash {
  std::size_t operator()(const Rule &r) const { return (std::size_t)r.hash(); }
};
typedef std::unordered_set<Rule, RuleHash> RuleSet;

struct RuleSetHash {

  std::size_t operator()(const RuleSet &rs) const {

    std::size_t in = 0;
    for (auto it = rs.begin(); it != rs.end(); it++) {
      in += it->hash();
    }
    return in;
  }
};
struct RuleSSetHash {

  std::size_t operator()(const std::unordered_set<std::string> &rs) const {
    std::hash<std::string> hasher;
    std::size_t in = 0;
    for (auto it = rs.begin(); it != rs.end(); it++) {
      in += hasher(*it);
    }
    return in;
  }
};
#endif // RULE_H_
