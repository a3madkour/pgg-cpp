#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include "GenChain.h"
#include "Rule.h"

struct RecipeEntry {
  std::string rule_name;
  int max;
  int min;
  int index;
  static std::string getStringFromInstance(const RecipeEntry &instance) {
    return std::to_string(instance.index);
  }
};
struct Grammar {

  typedef std::unordered_map<std::string, Graph::Vertex> VertexMap;
  typedef std::unordered_map<std::string, Rule> RuleMap;
  typedef std::unordered_map<std::string, std::string> LHSRHSMap;
  typedef std::pair<VertexMap, Rule> MatchedMap;
  typedef std::pair<VertexMap, LHSRHSMap> RHSnLHSMap;
  typedef std::pair<Graph, GenChain> GeneratedGraph;
  typedef std::unordered_map<std::string, double> RuleProb;
  typedef std::unordered_map<std::unordered_set<std::string>, RuleProb,
                             RuleSSetHash>
      RuleProbMap;
  typedef std::unordered_map<std::string, double> FinalProbs;
  Graph axiom;
  int max_applications;
  Graph intermidiate_graph;
  GenChain chain;
  std::vector<Rule> rules;
  RuleMap rule_map;
  RuleProbMap rule_prob_map;
  Grammar(Graph g, std::vector<Rule> rules) : axiom(g), rules(rules) {}
  FinalProbs computeProbs(std::vector<MatchedMap> &);
  Grammar();
  bool matchAndApplyRule(std::vector<Rule> &);
  Grammar(std::string jsonFile);
  GeneratedGraph generateGraph();
  GeneratedGraph applyRecipe(std::vector<RecipeEntry>, bool);
  std::vector<GenChain> loadChains(std::string);
  std::vector<MatchedMap> matchingRules(const std::vector<Rule> &);
  void initalizeGrammar(std::string);
  void applyRule(MatchedMap &, RHS &);
  void learnParameters(std::string);
  void learnParameters(std::vector<GenChain> &);
  RHSnLHSMap makeRHS(MatchedMap &, RHS &);
  void deleteMissingVertices(MatchedMap &, RHS &, RHSnLHSMap &);
  void deleteMissingEdges(MatchedMap &, RHS &, RHSnLHSMap &);
  void addMissingVertices(MatchedMap &, RHS &, RHSnLHSMap &);
  void addMissingEdges(MatchedMap &, RHS &, RHSnLHSMap &);

  static Graph makeGraph(const rapidjson::Value &);
};
#endif // GRAMMAR_H_
