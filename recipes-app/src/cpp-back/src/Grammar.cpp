#include "Grammar.h"
#include <boost/graph/vf2_sub_graph_iso.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <random>
#include <time.h>
#include <unordered_map>

Graph Grammar::makeGraph(const rapidjson::Value &graph) {

  Graph return_graph;
  const rapidjson::Value &nodes = graph["nodes"];
  const rapidjson::Value &edges = graph["edges"];
  std::unordered_map<std::string, Graph::Vertex> id_map;
  for (auto &v : nodes.GetArray()) {
    VertexProperties vp;
    vp.label = v["label"].GetString();
    vp.abbrev = v["abbrev"].GetString();
    vp.mark = v["mark"].GetString();
    vp.id = v["id"].GetString();
    Graph::Vertex node = return_graph.AddVertex(vp);
    id_map[vp.id] = node;
  }
  for (auto &e : edges.GetArray()) {
    std::string from = e["from"].GetString();
    std::string to = e["to"].GetString();
    Graph::Vertex u = id_map[from];
    Graph::Vertex v = id_map[to];
    return_graph.AddEdge(u, v);
  }
  return return_graph;
}

void Grammar::initalizeGrammar(std::string js) {

  chain.sample_id = intermidiate_graph.sample_id;
  rapidjson::Document document;
  max_applications = 20;
  document.Parse(js.c_str());
  axiom = makeGraph(document["axiom"]);

  const rapidjson::Value &rules_json = document["rules"];
  // std::cout << rules[0]["name"].GetString() << std::endl;
  for (auto &v : rules_json.GetArray()) {
    std::string rule_name = v["name"].GetString();
    Graph lhs_graph = makeGraph(v["lhs"]);
    LHS lhs = LHS(std::move(lhs_graph));
    std::vector<RHS> rhss;
    for (auto &r : v["rhs"].GetArray()) {
      double prob_rhs = r["probability"].GetDouble();
      Graph rhs_graph = makeGraph(r["graph"]);
      RHS rhs = RHS(std::move(rhs_graph), prob_rhs);
      rhss.push_back(std::move(rhs));
    }
    Rule rule = Rule(rule_name, std::move(lhs), std::move(rhss), 1.0);
    this->rule_map[rule.name] = rule;
    this->rules.push_back(std::move(rule));
  }
}
Grammar::Grammar(std::string jsonFile) { initalizeGrammar(jsonFile); }

Grammar::Grammar() {

  chain.sample_id = intermidiate_graph.sample_id;
  rapidjson::Document document;
  std::stringstream ss;
  max_applications = 20;
  std::ifstream file("dormans-grammar.json");
  if (file) {
    ss << file.rdbuf();
    file.close();
  } else {
    // std::cout << "Shit went down" << std::endl;
  }

  initalizeGrammar(ss.str());
}

std::vector<Grammar::MatchedMap>
Grammar::matchingRules(const std::vector<Rule> &m_rules) {
  std::vector<Grammar::MatchedMap> matched_rules_and_vertex_maps;
  matched_rules_and_vertex_maps.clear();
  // std::cout << "we are starting matchrules.size: "
  //           << matched_rules_and_vertex_maps.size() << std::endl;
  // std::cout << "we are starting m_rules.size: " << m_rules.size() <<
  // std::endl;
  for (const Rule &rule : m_rules) {
    auto cb = [&](auto &&f, auto &&g) {
      VertexMap v_map;
      // auto vi = get(boost::vertex_index, rule.lhs.graph.graph);
      // auto vi2 = get(boost::vertex_index, intermidiate_graph.graph);
      auto vp = get(vertex_properties, rule.lhs.graph.graph);
      // std::cout << "Considering rule: " << rule.name << std::endl;
      Graph::vertex_iter it, end;
      bool it_matches = true;
      for (boost::tie(it, end) = boost::vertices(rule.lhs.graph.graph);
           it != end; ++it) {
        auto wh = get(f, *it);
        // TOOD: Add ways for checking for wildcards
        if (vp[*it].abbrev != vp[wh].abbrev) {
          it_matches = false;
          break;
        }
        if (vp[*it].abbrev == vp[wh].abbrev) {

          // std::cout << "vp[*it].label" << std::endl;
          // std::cout << vp[*it].label << std::endl;
          // std::cout << "vp[wh].label" << std::endl;
          // std::cout << vp[wh].label << std::endl;
          // std::cout << "Matching: " << std::endl;
          // std::cout << "vp[*it].id" << std::endl;
          // std::cout << vp[*it].label << std::endl;
          // std::cout << "wh" << std::endl;
          // std::cout << wh << std::endl;
          v_map[vp[*it].id] = wh;
          // std::cout << "v_map[vp[*it].id]" << std::endl;
          // std::cout << v_map[vp[*it].id] << std::endl;
          // std::cout << "--------------------------------" << std::endl;
        }
      }

      if (it_matches) {
        matched_rules_and_vertex_maps.push_back(MatchedMap(v_map, rule));
      }
      return true;
    };

    boost::vf2_subgraph_iso(rule.lhs.graph.graph, intermidiate_graph.graph, cb);
  }

  return matched_rules_and_vertex_maps;
}

Grammar::GeneratedGraph Grammar::generateGraph() {

  intermidiate_graph.clear();
  chain.clearChain();
  chain.sample_id = intermidiate_graph.sample_id;
  intermidiate_graph = Graph(axiom);

  for (int i = 0; i < max_applications; i++) {

    // std::cout << "starting again matching?" << std::endl;
    std::vector<Grammar::MatchedMap> matched_rules = matchingRules(rules);
    FinalProbs fp = computeProbs(matched_rules);
    // std::cout << "matched_rules.size: " << matched_rules.size() <<
    // std::endl; std::cout << "fp.size: " << fp.size() << std::endl;
    std::vector<double> weights;
    std::unordered_set<std::string> rs;
    for (auto it = matched_rules.begin(); it != matched_rules.end(); it++) {
      // TODO: compute it from rule_prob_map
      weights.push_back(fp[it->second.name]);
      rs.insert(it->second.name);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::discrete_distribution<int> rule_d(weights.begin(), weights.end());

    // std::cout << "looking at rules" << std::endl;
    int num_valid_rules = matched_rules.size();
    // std::cout << "num_valid_rules: " << num_valid_rules << std::endl;
    if (num_valid_rules == 0) {
      // std::cout << "no more rules?" << std::endl;

      break;
    }
    // TODO pick rule based on probability
    int picked_rule_index = rule_d(generator);
    // std::cout << "rule index: " << picked_rule_index << std::endl;
    // std::cout << "rule_d max: " << rule_d.probabilities() << std::endl;
    MatchedMap picked = matched_rules[picked_rule_index];
    std::vector<double> rhs_weights;
    Rule &r = picked.second;
    std::vector<RHS> &rhss = r.rhss;

    // std::cout << "we are adding another node: " << r.name << std::endl;
    chain.addNode(r.name, rs);
    // std::cout << "we here???" << std::endl;
    for (auto it = rhss.begin(); it != rhss.end(); it++) {

      rhs_weights.push_back(it->prob);
    }
    // std::cout << "we here?sdasdasd??" << std::endl;
    std::discrete_distribution<int> rhs_d(rhs_weights.begin(),
                                          rhs_weights.end());
    int picked_rhs_index = rhs_d(generator);
    // std::cout << "rule name: " << picked.second.name << std::endl;
    // std::cout << picked.second.rhss.size() << std::endl;
    // int picked_rhs_index = rand() % rhss.size();
    // std::cout << "num_rhss: " << rhss.size() << std::endl;
    // std::cout << "rhs index: " << picked_rhs_index << std::endl;
    // std::cout << "--------------------------------" << std::endl;

    // std::cout << "rhs_d max: " << rhs_d.max() << std::endl;
    RHS &rhs = rhss[picked_rhs_index];

    // std::cout << "wwdadasde here?sdasdasd??" << std::endl;
    chain.addRuleApplication(r, picked_rhs_index);
    // std::cout << "intermidiate_graph before rule" << std::endl;
    // std::cout << intermidiate_graph << std::endl;

    // std::cout << "applying rule: " << picked.second.name << std::endl;
    applyRule(picked, rhs);
    // std::cout << "hdwadawdawdawdwd??" << std::endl;
    // std::cout << "intermidiate_graph after" << std::endl;
    // std::cout << intermidiate_graph << std::endl;

    // break;
    // std::cout << "Why are we dumping" << std::endl;
  }

  // std::cout << "hsdasdadasdasdasdasdsaddwadawdawdawdwd??" << std::endl;
  // intermidiate_graph.reindex();
  GeneratedGraph gg(std::move(intermidiate_graph), chain);
  return gg;
}

void Grammar::applyRule(MatchedMap &mm, RHS &rhs) {
  Grammar::RHSnLHSMap rhs_n_lhs_map = makeRHS(mm, rhs);
  deleteMissingVertices(mm, rhs, rhs_n_lhs_map);
  deleteMissingEdges(mm, rhs, rhs_n_lhs_map);
  addMissingVertices(mm, rhs, rhs_n_lhs_map);
  addMissingEdges(mm, rhs, rhs_n_lhs_map);
  // call delete vertices
  // call delete edges
  // call add vertices
  // call add edges
}
Grammar::RHSnLHSMap Grammar::makeRHS(MatchedMap &mm, RHS &rhs) {
  // std::cout << "picked_lhs_index" << std::endl;
  VertexMap &lhs_mapping = mm.first;
  VertexMap rhs_mapping;
  // std::cout << "makeRHS lhs mapping size: " << lhs_mapping.size() <<
  // std::endl;
  LHSRHSMap lhs_rhs_mapping;
  Rule &r = mm.second;
  // std::cout << lhs_mapping.size() << std::endl;
  // Pick a RHS
  // std::cout << "picked_rhs_index" << std::endl;
  Graph::vertex_iter it, end;
  Graph::vertex_iter itr, endr;
  auto vp = get(vertex_properties, r.lhs.graph.graph);
  for (boost::tie(it, end) = boost::vertices(r.lhs.graph.graph); it != end;
       ++it) {

    for (boost::tie(itr, endr) = boost::vertices(rhs.graph.graph); itr != endr;
         ++itr) {
      if (vp[*it].mark == vp[*itr].mark) {
        // std::cout << vp[*it].id << std::endl;
        // std::cout << vp[lhs_mapping[vp[*it].id]].id << std::endl;
        // std::cout << vp[*itr].label << std::endl;
        // std::cout << vp[*it].id << std::endl;
        // std::cout << vp[*it].label << std::endl;
        // std::cout << vp[lhs_mapping[*it]].id << std::endl;
        // std::cout << vp[lhs_mapping[*it]].label << std::endl;
        // if (lhs_mapping.find(*it) != lhs_mapping.end()) {
        //   std::cout << "we have it? " << std::endl;
        // }
        // std::cout << "LHS have it? " << std::endl;
        // std::cout << *it << std::endl;
        // std::cout << lhs_mapping[*it] << std::endl;

        // std::cout << "======================" << std::endl;
        rhs_mapping[vp[*itr].id] = lhs_mapping[vp[*it].id];
        lhs_rhs_mapping[vp[*it].id] = vp[*itr].id;
      }
    }
  }

  return Grammar::RHSnLHSMap(rhs_mapping, lhs_rhs_mapping);
}
void Grammar::deleteMissingVertices(Grammar::MatchedMap &mm, RHS &rhs,
                                    Grammar::RHSnLHSMap &rhs_n_lhs_map) {
  VertexMap &lhs_mapping = mm.first;
  // std::cout << "delete Missing Vertices lhs mapping size: "
  //           << lhs_mapping.size() << std::endl;
  Rule &r = mm.second;
  // VertexMap &rhs_mapping = rhs_n_lhs_map.first;
  LHSRHSMap &lhs_rhs_mapping = rhs_n_lhs_map.second;
  Graph::vertex_iter it, end;
  Graph::vertex_iter itr, endr;
  auto vp = get(vertex_properties, r.lhs.graph.graph);
  for (boost::tie(it, end) = boost::vertices(r.lhs.graph.graph); it != end;
       ++it) {
    std::string rhs_v_id = lhs_rhs_mapping[vp[*it].id];
    // std::cout << "rhs_v_id" << std::endl;
    // std::cout << rhs_v_id << std::endl;
    bool exists = false;
    for (boost::tie(itr, endr) = boost::vertices(rhs.graph.graph); itr != endr;
         ++itr) {

      if (rhs_v_id == vp[*itr].id) {
        // std::cout << "exist" << std::endl;
        exists = true;
      }
    }
    if (!exists) {

      // std::cout << "deactivated" << std::endl;
      intermidiate_graph.RemoveVertex(lhs_mapping[vp[*it].id]);
    }
  }
}
void Grammar::deleteMissingEdges(Grammar::MatchedMap &mm, RHS &rhs,
                                 Grammar::RHSnLHSMap &rhs_n_lhs_map) {
  VertexMap &lhs_mapping = mm.first;
  Rule &r = mm.second;
  // VertexMap &rhs_mapping = rhs_n_lhs_map.first;
  // LHSRHSMap &lhs_rhs_mapping = rhs_n_lhs_map.second;
  Graph::edge_iter it, end;
  Graph::edge_iter itr, endr;
  // std::cout << "delete Missing Edges lhs mapping size: " <<
  // lhs_mapping.size()
  //           << std::endl;
  auto vp = get(vertex_properties, r.lhs.graph.graph);
  for (tie(it, end) = boost::edges(r.lhs.graph.graph); it != end; ++it) {
    bool exists = false;
    Graph::Vertex lhs_src = boost::source(*it, r.lhs.graph.graph);
    Graph::Vertex lhs_end = boost::target(*it, r.lhs.graph.graph);
    // std::cout << vp[lhs_src].label << std::endl;
    // std::cout << vp[lhs_src].id << std::endl;
    Graph::Vertex lhs_graph_start_id = lhs_mapping[vp[lhs_src].id];
    // std::cout << lhs_graph_start_id << std::endl;
    Graph::Vertex lhs_graph_end_id = lhs_mapping[vp[lhs_end].id];
    // std::cout << lhs_graph_end_id << std::endl;
    for (tie(itr, endr) = boost::edges(rhs.graph.graph); itr != endr; ++itr) {
      Graph::Vertex rhs_src = boost::source(*itr, rhs.graph.graph);
      Graph::Vertex rhs_end = boost::target(*itr, rhs.graph.graph);

      if (vp[lhs_graph_start_id].abbrev == vp[rhs_src].abbrev &&
          vp[lhs_graph_end_id].abbrev == vp[rhs_end].abbrev) {
        if (vp[lhs_graph_start_id].abbrev == "F" &&
            vp[rhs_end].abbrev == "km") {

          // std::cout << "how does this exists?" << std::endl;
        }
        exists = true;
        break;
      }
    }
    if (!exists) {
      Graph::edge_iter itg, endg;
      for (tie(itg, endg) = boost::edges(intermidiate_graph.graph); itg != endg;
           ++itg) {

        Graph::Vertex int_graph_start_id =
            boost::source(*itg, intermidiate_graph.graph);
        Graph::Vertex int_graph_end_id =
            boost::target(*itg, intermidiate_graph.graph);

        if (vp[lhs_graph_start_id].id == vp[int_graph_start_id].id) {
          if (vp[lhs_graph_end_id].id == vp[int_graph_end_id].id) {
            // std::cout << "We popping you off:" << std::endl;
            // std::cout << vp[int_graph_start_id].label << "->"
            //           << vp[int_graph_end_id].label << std::endl;
            // std::cout << vp[int_graph_start_id].id << "->"
            //           << vp[int_graph_end_id].id << std::endl;
            boost::remove_edge(*itg, intermidiate_graph.graph);
            break;
          }
        }
      }
    }
  }
}
void Grammar::addMissingVertices(Grammar::MatchedMap &mm, RHS &rhs,
                                 Grammar::RHSnLHSMap &rhs_n_lhs_map) {
  // bool vertex_exists = false;
  // VertexMap &lhs_mapping = std::get<0>(mm);
  Rule &r = mm.second;
  VertexMap &rhs_mapping = rhs_n_lhs_map.first;
  // LHSRHSMap &lhs_rhs_mapping = rhs_n_lhs_map.second;
  Graph::vertex_iter it, end;
  Graph::vertex_iter itr, endr;
  // std::cout << "make it count baby" << std::endl;
  // std::cout << intermidiate_graph.getVertexCount() << std::endl;
  auto vp = get(vertex_properties, r.lhs.graph.graph);
  for (boost::tie(itr, endr) = boost::vertices(rhs.graph.graph); itr != endr;
       ++itr) {

    if (rhs_mapping.find(vp[*itr].id) == rhs_mapping.end()) {
      // std::cout << "I did not find the vertex" << std::endl;
      // std::cout << "adding vertex" << std::endl;
      // std::cout << vp[*itr].id << std::endl;
      // std::cout << vp[*itr].label << std::endl;
      // std::cout << "Done adding vertex" << std::endl;
      Graph::Vertex v =
          intermidiate_graph.AddVertex(VertexProperties(vp[*itr]));
      boost::uuids::uuid uid = boost::uuids::random_generator()();
      std::string ids = boost::uuids::to_string(uid);
      // vp[v].id = ids;

      // std::cout << "adding vertex" << std::endl;
      // std::cout << vp[*itr].id << std::endl;
      // std::cout << vp[v].id << std::endl;
      // std::cout << "vdasertex" << std::endl;

      rhs_mapping[vp[*itr].id] = v;
      vp[v].id = ids;

    } else {
      Graph::Vertex graph_start_id = rhs_mapping[vp[*itr].id];
      boost::uuids::uuid uid = boost::uuids::random_generator()();
      std::string ids = boost::uuids::to_string(uid);

      // set graph vertex properties to rhs properties
      //
      // std::cout << "copy vertex property" << std::endl;
      // std::cout << vp[*itr].id << std::endl;
      vp[graph_start_id] = VertexProperties(vp[*itr]);
      vp[graph_start_id].id = ids;
    }
  }
  return;
}
void Grammar::addMissingEdges(Grammar::MatchedMap &mm, RHS &rhs,
                              Grammar::RHSnLHSMap &rhs_n_lhs_map) {
  VertexMap lhs_mapping = mm.first;
  // Rule &r = std::get<1>(mm);
  VertexMap &rhs_mapping = rhs_n_lhs_map.first;
  // LHSRHSMap &lhs_rhs_mapping = rhs_n_lhs_map.second;
  Graph::edge_iter it, end;
  Graph::edge_iter itr, endr;
  Graph::edge_iter itg, endg;
  auto vp = get(vertex_properties, intermidiate_graph.graph);
  for (tie(itr, endr) = boost::edges(rhs.graph.graph); itr != endr; ++itr) {
    bool edge_exists = false;
    Graph::Vertex rhs_src = boost::source(*itr, rhs.graph.graph);
    Graph::Vertex rhs_end = boost::target(*itr, rhs.graph.graph);
    // std::cout << vp[rhs_end].id << std::endl;
    // std::cout << vp[rhs_end].label << std::endl;
    Graph::Vertex graph_start_id = rhs_mapping[vp[rhs_src].id];
    Graph::Vertex graph_end_id = rhs_mapping[vp[rhs_end].id];

    // std::cout << "make it count" << std::endl;
    // std::cout << intermidiate_graph.getVertexCount() << std::endl;
    // std::cout << vp[graph_start_id].id << std::endl;
    // std::cout << "run it twice" << std::endl;
    // std::cout << vp[rhs_src].id << std::endl;
    // std::cout << vp[rhs_end].id << std::endl;
    // std::cout << "run it tonight" << std::endl;
    // std::cout << vp[graph_end_id].id << std::endl;

    for (tie(itg, endg) = boost::edges(intermidiate_graph.graph); itg != endg;
         ++itg) {
      Graph::Vertex temp_start_id =
          std::move(boost::source(*itg, intermidiate_graph.graph));
      Graph::Vertex temp_end_id =
          std::move(boost::target(*itg, intermidiate_graph.graph));
      if (vp[graph_start_id].id == vp[temp_start_id].id &&
          vp[graph_end_id].id == vp[temp_end_id].id) {
        edge_exists = true;
        // std::cout << "wait this exists? " << std::endl;
        // std::cout << vp[graph_start_id].label << "->" <<
        // vp[graph_end_id].label
        // << std::endl;
        break;
      }
    }
    if (!edge_exists) {
      // std::cout << vp[graph_start_id].label << "->" <<
      // vp[graph_end_id].label
      //           << std::endl;
      if (vp[graph_start_id].id != vp[graph_end_id].id) {
        intermidiate_graph.AddEdge(graph_start_id, graph_end_id);
      }
      // std::cout << intermidiate_graph << std::endl;
    }
  }
  return;
}

bool Grammar::matchAndApplyRule(std::vector<Rule> &mm_rules) {
  std::vector<Grammar::MatchedMap> matched_rules = matchingRules(mm_rules);
  FinalProbs fp = computeProbs(matched_rules);
  std::vector<double> weights;
  std::unordered_set<std::string> rs;
  for (auto it = matched_rules.begin(); it != matched_rules.end(); it++) {
    weights.push_back(fp[it->second.name]);
    rs.insert(it->second.name);
  }
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  std::discrete_distribution<int> rule_d(weights.begin(), weights.end());

  // std::cout << "looking at rules" << std::endl;
  int num_valid_rules = matched_rules.size();
  // std::cout << "num_valid_rules: " << num_valid_rules << std::endl;
  if (num_valid_rules == 0) {
    // std::cout << "no more rules?" << std::endl;
    return true;
  }

  int picked_rule_index = rule_d(generator);
  // std::cout << "rule index: " << picked_rule_index << std::endl;
  // std::cout << "rule_d max: " << rule_d.probabilities() << std::endl;
  MatchedMap picked = matched_rules[picked_rule_index];
  std::vector<double> rhs_weights;
  Rule &r = picked.second;
  std::vector<RHS> &rhss = r.rhss;

  // std::cout << "we are adding another node: " << r.name << std::endl;
  chain.addNode(r.name, rs);
  // std::cout << "we here???" << std::endl;
  for (auto it = rhss.begin(); it != rhss.end(); it++) {

    rhs_weights.push_back(it->prob);
  }
  // std::cout << "we here?sdasdasd??" << std::endl;
  std::discrete_distribution<int> rhs_d(rhs_weights.begin(), rhs_weights.end());
  int picked_rhs_index = rhs_d(generator);
  // std::cout << "rule name: " << picked.second.name << std::endl;
  // std::cout << picked.second.rhss.size() << std::endl;
  // int picked_rhs_index = rand() % rhss.size();
  // std::cout << "num_rhss: " << rhss.size() << std::endl;
  // std::cout << "rhs index: " << picked_rhs_index << std::endl;
  // std::cout << "--------------------------------" << std::endl;

  // std::cout << "rhs_d max: " << rhs_d.max() << std::endl;
  RHS &rhs = rhss[picked_rhs_index];

  // std::cout << "wwdadasde here?sdasdasd??" << std::endl;
  // chain.addRuleApplication(r, picked_rhs_index);
  // std::cout << "intermidiate_graph before rule" << std::endl;
  // std::cout << intermidiate_graph << std::endl;

  chain.addRuleApplication(r, picked_rhs_index);
  // std::cout << "applying rule: " << picked.second.name << std::endl;
  applyRule(picked, rhs);
  return false;
}
Grammar::GeneratedGraph Grammar::applyRecipe(std::vector<RecipeEntry> re,
                                             bool cont) {
  intermidiate_graph.clear();
  chain.clearChain();
  chain.sample_id = intermidiate_graph.sample_id;
  intermidiate_graph = Graph(axiom);

  for (auto it = re.begin(); it != re.end(); it++) {
    // std::cout << "IDM" << std::endl;
    if (rule_map.find(it->rule_name) == rule_map.end()) {
      // std::cout << it->rule_name << " does not exist in the grammar"
      // << std::endl;
    }
    std::vector<Rule> mm_rules;
    mm_rules.push_back(rule_map[it->rule_name]);
    int num_app = rand() % it->max + it->min;
    bool _break = false;
    for (int i = 0; i < num_app; i++) {
      _break = matchAndApplyRule(mm_rules);
      if (_break) {
        break;
      }
    }
  }
  if (cont) {
    bool _break = false;
    for (int i = 0; i < max_applications; i++) {
      _break = matchAndApplyRule(rules);
      if (_break) {
        break;
      }
    }
  }
  // intermidiate_graph.reindex();
  GeneratedGraph gg(std::move(intermidiate_graph), chain);
  return gg;
}

std::vector<GenChain> Grammar::loadChains(std::string path) {
  std::vector<GenChain> chains;
  // for (const auto &file : std::filesystem::directory_iterator(path)) {
  //   // std::cout << file.path() << std::endl;
  //   GenChain ch = GenChain::load(file.path());
  //   // std::cout << "here" << std::endl;
  //   chains.push_back(ch);
  //   // std::cout << "50 cent" << std::endl;
  // }

  // // std::cout << "50 cent I guess" << std::endl;
  return chains;
}

void Grammar::learnParameters(std::string path) {
  std::vector<GenChain> chains;
  chains = loadChains(path);
  learnParameters(chains);
  return;
}
void Grammar::learnParameters(std::vector<GenChain> &gcs) {

  RuleSetMap total_rule_set_map;
  RuleAppMap total_rule_map;
  for (const auto &chain : gcs) {
    for (const auto &rule_set_pair : chain.rule_set_map) {
      const std::unordered_set<std::string> &rule_set = rule_set_pair.first;
      if (total_rule_set_map.find(rule_set) != total_rule_set_map.end()) {
        RuleApplication ra =
            RuleApplication(std::unordered_map<std::string, int>(), 0);
        total_rule_set_map[rule_set] = ra;
      }

      for (const auto &rule : rule_set) {
        // std::cout << rule.name << std::endl;
        if (total_rule_set_map[rule_set].first.find(rule) !=
            total_rule_set_map[rule_set].first.end()) {
          total_rule_set_map[rule_set].first[rule] = 0;
        }
      }
      for (const auto &rule_pair : chain.rule_set_map.at(rule_set).first) {
        std::string rule_id = rule_pair.first;
        total_rule_set_map[rule_set].first[rule_id] +=
            chain.rule_set_map.at(rule_set).first.at(rule_id);
      }
      total_rule_set_map[rule_set].second +=
          chain.rule_set_map.at(rule_set).second;
    }

    for (const auto &rule_name_pair : chain.rule_map) {
      std::string rule_name = rule_name_pair.first;
      if (total_rule_map.find(rule_name) != total_rule_map.end()) {
        RuleApplication ra =
            RuleApplication(std::unordered_map<std::string, int>(), 0);
        total_rule_map[rule_name] = ra;
      }

      for (const auto &rhs_index_pair : chain.rule_map.at(rule_name).first) {
        std::string rhs_index = rhs_index_pair.first;
        if (total_rule_map[rule_name].first.find(rhs_index) !=
            total_rule_map[rule_name].first.end()) {
          total_rule_map[rule_name].first[rhs_index] +=
              chain.rule_map.at(rule_name).first.at(rhs_index);
        } else {
          total_rule_map[rule_name].first[rhs_index] =
              chain.rule_map.at(rule_name).first.at(rhs_index);
        }
      }

      total_rule_map[rule_name].second += chain.rule_map.at(rule_name).second;
    }

    for (auto &rule : rules) {
      if (total_rule_map.find(rule.name) != total_rule_map.end()) {
        int i = 0;
        for (auto &rhs : rule.rhss) {
          rhs.prob =
              (double)total_rule_map[rule.name].first[std::to_string(i)] /
              total_rule_map[rule.name].second;
          i++;
        }
      }
    }

    for (auto &rule_set_pair : total_rule_set_map) {
      const std::unordered_set<std::string> &rs = rule_set_pair.first;
      if (rule_prob_map.find(rs) != rule_prob_map.end()) {
        rule_prob_map[rs] = RuleProb();
      }
      for (auto &rule_name_pair : total_rule_set_map[rs].first) {
        std::string rule_name = rule_name_pair.first;
        rule_prob_map[rs][rule_name] =
            (double)rule_name_pair.second / total_rule_set_map[rs].second;
      }
    }
  }

  return;
}

Grammar::FinalProbs Grammar::computeProbs(std::vector<MatchedMap> &mms) {

  RuleProb rule_probs;
  std::unordered_set<std::string> rule_names;

  std::unordered_map<std::string, RuleProb> rule_map_duplicate;

  int i = 0;
  // std::cout << "mms.size(): " << mms.size() << std::endl;
  for (auto &mm : mms) {
    Rule &r = mm.second;
    double prob = r.prob;
    rule_probs[std::to_string(i)] = prob;
    rule_names.insert(r.name);
    if (rule_map_duplicate.find(r.name) == rule_map_duplicate.end()) {
      RuleProb prob_vec;
      rule_map_duplicate[r.name] = prob_vec;
    }
    rule_map_duplicate[r.name][std::to_string(i)] = prob;
    i++;
  }
  if (rule_prob_map.find(rule_names) != rule_prob_map.end()) {
    RuleProb &rb = rule_prob_map[rule_names];
    for (auto &rb_pair : rb) {

      std::string rule_name_p = rb_pair.first;
      double rule_p = rb_pair.second;
      RuleProb &rule_prob_p = rule_map_duplicate[rule_name_p];
      for (auto &prob_it : rule_prob_p) {
        std::string index = prob_it.first;
        rule_prob_p[index] = rule_p;
      }
    }
  }

  double sum_rule_probs = 0;
  for (auto &it : rule_map_duplicate) {
    std::string rule_name_p = it.first;
    RuleProb r_p_map_p = it.second;
    int num_rule_matches = r_p_map_p.size();
    for (auto &prob_it_p : r_p_map_p) {
      std::string index = prob_it_p.first;
      double prob_p = prob_it_p.second;
      rule_probs[index] = (double)prob_p / num_rule_matches;
      sum_rule_probs += (double)prob_p / num_rule_matches;
    }
  }

  FinalProbs final_probs;
  // std::cout << "rule_probs_size: " << rule_probs.size() << std::endl;
  for (auto &rule_prob_it : rule_probs) {
    std::string rule_index_p = rule_prob_it.first;
    double &rule_p = rule_prob_it.second;
    final_probs[rule_index_p] = (double)rule_p / sum_rule_probs;
  }

  return final_probs;
}
