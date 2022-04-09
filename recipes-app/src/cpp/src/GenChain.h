#ifndef GENCHAIN_H_
#define GENCHAIN_H_

#include "Node.h"
#include "document.h"
struct GenChain {

  std::string sample_id;
  std::vector<Node> chain;
  RuleAppMap rule_map;
  RuleSetMap rule_set_map;
  double prob;
  GenChain(std::string, double);
  GenChain(std::string);
  GenChain();
  GenChain(const GenChain &gc);
  ~GenChain();
  void addNode(std::string, std::unordered_set<std::string> &);
  void save(std::string);
  std::string toJSON();
  static GenChain load(std::string);
  void addRuleApplication(Rule &, int);
  void clearChain();
  friend std::ostream &operator<<(std::ostream &, const GenChain &);
};
#endif // GENERATIONCHAIN_H_
