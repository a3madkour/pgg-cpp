#include "Grammar.h"
#include "VisGraph.h"
#include <emscripten/bind.h>

using namespace emscripten;

float lerp(float a, float b, float t) { return (1 - t) * a + t * b; }
VisGraph generate(std::string json_file) {
  Grammar gr(json_file);
  Grammar::GeneratedGraph gg = gr.generateGraph();
  Graph graph = gg.first;
  VisGraph g = graph.toVisGraph();
  g.chain_str = gg.second.toJSON();
  return g;
}

std::vector<VisGraph> generate_n(std::string json_file, int n) {
  Grammar gr(json_file);
  std::vector<VisGraph> gs;
  for (int i = 0; i < n; i++) {
    Grammar::GeneratedGraph gg = gr.generateGraph();
    Graph &graph = gg.first;
    VisGraph g = graph.toVisGraph();
    g.chain_str = gg.second.toJSON();
    gs.push_back(g);
  }
  return gs;
}

std::vector<VisGraph>
generate_n_learnt_chains(std::string json_file, int n,
                         std::vector<std::string> chains) {
  Grammar gr(json_file);
  std::vector<GenChain> gcs;
  for (std::string chain_str : chains) {
    gcs.push_back(GenChain(chain_str));
  }
  gr.learnParameters(gcs);
  std::vector<VisGraph> gs;
  for (int i = 0; i < n; i++) {
    Grammar::GeneratedGraph gg = gr.generateGraph();
    Graph &graph = gg.first;
    VisGraph g = graph.toVisGraph();
    g.chain_str = gg.second.toJSON();
    gs.push_back(g);
  }
  return gs;
}

VisGraph apply_recipe(std::string json_file, std::vector<RecipeEntry> recipe) {
  Grammar gr(json_file);
  Grammar::GeneratedGraph gg = gr.applyRecipe(recipe, false);
  Graph &graph = gg.first;
  std::vector<Grammar::MatchedMap> mm = gr.matchingRules(gr.rules);
  std::vector<std::string> app_rules;
  for (auto &m_m : mm) {
    Rule &r = m_m.second;
    app_rules.push_back(r.name);
  }
  VisGraph g = graph.toVisGraph();
  g.app_rules = app_rules;
  return g;
}
std::vector<VisGraph> apply_recipe_n(std::string json_file,
                                     std::vector<RecipeEntry> recipe, int n,
                                     bool cont) {
  Grammar gr(json_file);
  std::vector<VisGraph> gs;
  for (int i = 0; i < n; i++) {
    Grammar::GeneratedGraph gg = gr.applyRecipe(recipe, cont);
    Graph &graph = gg.first;
    std::vector<Grammar::MatchedMap> mm = gr.matchingRules(gr.rules);
    std::vector<std::string> app_rules;
    for (auto &m_m : mm) {
      Rule &r = m_m.second;
      app_rules.push_back(r.name);
    }
    VisGraph g = graph.toVisGraph();
    g.app_rules = app_rules;
    gs.push_back(g);
  }
  return gs;
}

EMSCRIPTEN_BINDINGS(Module) {
  function("lerp", &lerp);
  function("generate", &generate);
  function("generate_n", &generate_n);
  function("generate_n_learnt_chains", &generate_n_learnt_chains);
  function("apply_recipe", &apply_recipe);
  function("apply_recipe_n", &apply_recipe_n);
  register_vector<std::string>("StringList");
  class_<VisNode>("VisNode")
      .constructor()
      .property("id", &VisNode::id)
      .property("index", &VisNode::index)
      .property("abbrev", &VisNode::abbrev)
      .property("label", &VisNode::label)
      .property("mark", &VisNode::mark)
      .class_function("getStringFromInstance", &VisNode::getStringFromInstance);
  class_<VisEdge>("VisEdge")
      .constructor()
      .property("from", &VisEdge::from)
      .property("to", &VisEdge::to)
      .class_function("getStringFromInstance", &VisEdge::getStringFromInstance);
  register_vector<VisNode>("nodes");
  register_vector<VisEdge>("edges");
  class_<VisGraph>("VisGraph")
      .constructor()
      .property("id", &VisGraph::id)
      .property("nodes", &VisGraph::nodes)
      .property("edges", &VisGraph::edges)
      .property("leniency", &VisGraph::leniency)
      .property("mission_linearity", &VisGraph::mission_linearity)
      .property("map_linearity", &VisGraph::map_linearity)
      .property("path_redundancy", &VisGraph::path_redundancy)
      .property("chain_str", &VisGraph::chain_str)
      .property("app_rules", &VisGraph::app_rules)
      .property("app_rules", &VisGraph::app_rules)
      .class_function("getStringFromInstance",
                      &VisGraph::getStringFromInstance);
  register_vector<VisGraph>("graphs");
  class_<RecipeEntry>("RecipeEntry")
      .constructor()
      .property("rule_name", &RecipeEntry::rule_name)
      .property("index", &RecipeEntry::index)
      .property("max", &RecipeEntry::max)
      .property("min", &RecipeEntry::min)
      .class_function("getStringFromInstance",
                      &RecipeEntry::getStringFromInstance);
  register_vector<RecipeEntry>("recipe");
}
