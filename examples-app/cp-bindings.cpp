#include "Grammar.h"
#include "VisGraph.h"
#include <emscripten/bind.h>
#include <thread>

using namespace emscripten;

typedef struct args_struct {
  int begin;
  int end;
  std::vector<VisGraph> *samples;
  Grammar gr;
} argz;

void *generate_graph(void *arguments) {
  std::cout << "asda" << std::endl;
  struct args_struct *args = static_cast<struct args_struct *>(arguments);
  std::cout << args->begin << " " << args->end << std::endl;
  std::cout << "asda 2" << std::endl;
  Grammar &gr = args->gr;
  std::cout << "asda 23" << std::endl;
  std::cout << args->begin << " " << args->end << std::endl;
  for (int i = args->begin; i < args->end; i++) {
    std::cout << "asdadadasdasdasda dealing with me asdasdasdasda" << std::endl;
    Grammar::GeneratedGraph new_g_pair = gr.generateGraph();
    std::cout << "asdadadasdasdasda dealing with me made the graphs"
              << std::endl;

    Graph &graph = new_g_pair.first;
    VisGraph g = graph.toVisGraph();
    g.chain_str = new_g_pair.second.toJSON();
    (*args->samples)[i] = g;
  }
  delete args;
  return NULL;
}
// void generate_graph(std::string json_file, int begin, int end,
//                     std::vector<VisGraph> &samples) {
//   for (int i = begin; i < end; i++) {
//     Grammar::GeneratedGraph new_g_pair = gr.generateGraph();

//     samples[i] = g;
//   }
// }

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

  std::vector<VisGraph> gs(n);
  int num_threads = 2;
  pthread_t threads[num_threads];
  int samples_per_thread = n / num_threads;
  for (long int i = 0; i < num_threads; ++i) {
    int begin = i * samples_per_thread;
    int end = begin + samples_per_thread;
    if (i == num_threads - 1) {
      end = n;
    }
    argz *args = new argz;
    args->begin = begin;
    args->end = end;
    args->samples = &gs;
    args->gr.initalizeGrammar(json_file);

    std::cout << "what is going on" << std::endl;
    int t = pthread_create(&threads[i], NULL, &generate_graph, args);
    if (t != 0) {

      std::cout << "Error in thread create: " << t << std::endl;
    }
  }
  for (int i = 0; i < num_threads; ++i) {
    void *status;
    int t = pthread_join(threads[i], &status);
    if (t != 0) {
      std::cout << "Error in thread join: " << t << std::endl;
    }
  }

  // std::vector<VisGraph> gs;
  // for (int i = 0; i < n; i++) {
  //   Grammar::GeneratedGraph gg = gr.generateGraph();
  //   Graph &graph = gg.first;
  //   VisGraph g = graph.toVisGraph();
  //   g.chain_str = gg.second.toJSON();
  //   gs.push_back(g);
  // }
  std::cout << "sdadas dsada asda" << std::endl;
  std::cout << gs.size() << std::endl;
  std::cout << "The size ^" << std::endl;
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
      .property("hash", &VisGraph::hash)
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
