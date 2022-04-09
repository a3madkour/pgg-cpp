#include "Rule.h"

Rule::Rule() : name("null"), prob(1) {}
Rule::Rule(std::string name, LHS lhs, std::vector<RHS> rhss, double prob)
    : name(name), lhs(std::move(lhs)), rhss(std::move(rhss)), prob(prob) {}

std::ostream &operator<<(std::ostream &os, const Rule &rule) {
  os << "Rule " << rule.name << " :\n";
  os << "LHS: " << rule.lhs << std::endl;
  os << "RHSs: " << std::endl;
  for (auto it = rule.rhss.begin(); it != rule.rhss.end(); it++) {
    os << *it << std::endl;
  }

  return os;
}

std::size_t Rule::hash() const { return hasher(name); }

bool operator==(const Rule &left, const Rule &right) {
  return left.name == right.name;
}
