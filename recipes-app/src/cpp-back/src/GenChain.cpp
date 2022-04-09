#include "GenChain.h"

GenChain::GenChain() {

  boost::uuids::uuid uid = boost::uuids::random_generator()();
  sample_id = boost::uuids::to_string(uid);
  prob = 1;
}
GenChain::~GenChain() {
  // std::cout << "Something is very deleting" << std::endl;
  // std::cout << "inital deleting" << std::endl;
  // if (current_node != nullptr) {
  // std::cout << "Something is very wrong" << std::endl;
  // }
  chain.clear();
  rule_map.clear();
  // std::cout << "Review my shit fantano" << std::endl;
  rule_set_map.clear();
  // std::cout << "Review your shit fantano" << std::endl;
}
GenChain::GenChain(std::string s_id, double p) : sample_id(s_id), prob(p) {}
GenChain::GenChain(std::string js) {
  // std::cout << js << std::endl;
  rapidjson::Document document;
  document.Parse(js.c_str());
  // for (auto it = document.MemberBegin(); it != document.MemberEnd(); it++) {
  //   std::cout << "key: " << it->name.GetString() << std::endl;
  //   std::cout << "is object: " << document[it->name.GetString()].IsInt()
  //             << std::endl;
  // }
  // std::cout << document.GetStringLength() << std::endl;
  // std::cout << document["rule_sets"].IsObject() << std::endl;
  sample_id = document["sample_id"].GetString();
  prob = document["prob"].GetDouble();

  std::unordered_map<std::string, std::unordered_set<std::string>>
      rule_index_map;
  const rapidjson::Value &rule_sets_js = document["rule_sets"];
  const rapidjson::Value &chain_js = document["chain"];
  const rapidjson::Value &rule_map_js = document["rule_map"];
  const rapidjson::Value &rule_set_map_js = document["rule_set_map"];

  for (auto it = rule_sets_js.MemberBegin(); it != rule_sets_js.MemberEnd();
       it++) {
    std::unordered_set<std::string> rs;
    std::string index = it->name.GetString();
    auto rule_set_arr = rule_sets_js[it->name.GetString()].GetArray();
    for (auto itr = rule_set_arr.begin(); itr != rule_set_arr.end(); itr++) {
      rs.insert(itr->GetString());
    }
    rule_index_map[index] = rs;
  }
  auto chain_arr = chain_js.GetArray();
  for (auto it = chain_arr.begin(); it != chain_arr.end(); it++) {
    // std::cout << "We are loading a jsom baby" << std::endl;
    std::string rule_id = (*it)["rule_id"].GetString();
    std::string rule_set_index = (*it)["rule_set_index"].GetString();
    std::string rhs_i = (*it)["rhs_i"].GetString();
    std::unordered_set<std::string> sfsa = rule_index_map[rule_set_index];
    Node n(rule_id, sfsa, std::stoi(rhs_i));
    chain.push_back(n);
  }

  for (auto it = rule_map_js.MemberBegin(); it != rule_map_js.MemberEnd();
       it++) {
    std::string rule_name = it->name.GetString();
    // std::cout << "Rule name: " << rule_name << std::endl;
    const rapidjson::Value &rule_app_map_js =
        rule_map_js[it->name]["rule_app_map"];
    std::unordered_map<std::string, int> e;
    for (auto itr = rule_app_map_js.MemberBegin();
         itr != rule_app_map_js.MemberEnd(); itr++) {
      int app_count = rule_app_map_js[itr->name].GetInt();
      e[itr->name.GetString()] = app_count;
    }
    int count = rule_map_js[it->name]["count"].GetInt();
    RuleApplication rp(e, count);
    rule_map[rule_name] = rp;
  }

  for (auto it = rule_set_map_js.MemberBegin();
       it != rule_set_map_js.MemberEnd(); it++) {
    const rapidjson::Value &rule_app_map_js =
        rule_set_map_js[it->name]["rule_app_map"];
    std::unordered_map<std::string, int> e;
    for (auto itr = rule_app_map_js.MemberBegin();
         itr != rule_app_map_js.MemberEnd(); itr++) {
      int app_count = rule_app_map_js[itr->name].GetInt();
      e[itr->name.GetString()] = app_count;
    }
    int count = rule_set_map_js[it->name]["count"].GetInt();
    RuleApplication rp(e, count);
    rule_index_map[it->name.GetString()];
    rule_set_map[rule_index_map[it->name.GetString()]] = rp;
  }
}

GenChain::GenChain(const GenChain &gc) {

  sample_id = gc.sample_id;
  rule_map = gc.rule_map;
  prob = gc.prob;
  rule_set_map = gc.rule_set_map;
  chain = gc.chain;
}
void GenChain::addNode(std::string rule_id,
                       std::unordered_set<std::string> &rule_set) {
  // std::cout << "we addNode dumpiung" << std::endl;
  chain.push_back(Node(rule_id, rule_set));
  // std::cout << "we dumpiung by scope>" << std::endl;

  if (rule_set_map.find(rule_set) == rule_set_map.end()) {
    // std::cout << "Rule set not in map" << std::endl;
    RuleApplication ra =
        RuleApplication(std::unordered_map<std::string, int>(), 0);
    rule_set_map[rule_set] = ra;
  }
  if (rule_set_map[rule_set].first.find(rule_id) ==
      rule_set_map[rule_set].first.end()) {
    // std::cout << "Rule id not in rule set map" << std::endl;
    rule_set_map[rule_set].first[rule_id] = 1;
  } else {
    // std::cout << "Rule id in rule set map" << std::endl;
    rule_set_map[rule_set].first[rule_id]++;
  }
  // std::cout << "we dumpiung" << std::endl;

  rule_set_map[rule_set].second++;
  // std::cout << "we here" << std::endl;
  return;
}
void GenChain::save(std::string filename) {

  // std::ofstream file(filename);
  // {
  //   boost::archive::text_oarchive oa{file};
  //   oa << *this;
  // }
  // // boost::archive::text_oarchive oa{file};
  // // oa << *this;
  // file.close();

  // // save file
}
// TODO: This is dump make it better do not use strings use ints
void GenChain::addRuleApplication(Rule &r, int selected_rhs_index) {
  if (rule_map.find(r.name) == rule_map.end()) {
    RuleApplication ra =
        RuleApplication(std::unordered_map<std::string, int>(), 0);
    rule_map[r.name] = ra;
    int index = 0;
    for (auto it = r.rhss.begin(); it != r.rhss.end(); it++) {
      rule_map[r.name].first[std::to_string(index)] = 0;
      index++;
    }
  }

  rule_map[r.name].first[std::to_string(selected_rhs_index)]++;
  rule_map[r.name].second++;
  chain[chain.size() - 1].rhs_index = selected_rhs_index;
  return;
}

std::string GenChain::toJSON() {
  std::stringstream ss;

  std::unordered_map<std::unordered_set<std::string>, int, RuleSSetHash>
      rule_index_map;

  ss << "{ \n";
  ss << "\"prob\":" << prob << ",\n";
  ss << "\"sample_id\":\"" << sample_id << "\",\n";
  ss << "\"rule_sets\": {\n";
  int index = 0;
  for (auto it = rule_set_map.begin(); it != rule_set_map.end(); it++) {
    const std::unordered_set<std::string> &rule_set = it->first;
    ss << "\"" << index << "\": [\n";
    rule_index_map[rule_set] = index;
    for (auto itr = rule_set.begin(); itr != rule_set.end(); itr++) {
      ss << "\"" << *itr << "\",";
    }
    ss.seekp(-1, std::ios_base::end);
    ss << "\n],";
    index++;
  }
  ss.seekp(-1, std::ios_base::end);
  ss << "\n},\n";
  ss << "\"chain\":[\n";

  for (auto it = chain.begin(); it != chain.end(); it++) {
    ss << it->toJSON(rule_index_map);
    if (it != chain.end() - 1) {
      ss << ",";
    }
    ss << "\n";
  }
  ss << "],\n";
  ss << "\"rule_map\": {\n";
  for (auto it = rule_map.begin(); it != rule_map.end(); it++) {
    const std::string rule_name = it->first;
    const RuleApplication &rule_app = it->second;
    const std::unordered_map<std::string, int> &rp = rule_app.first;
    ss << "\"" << rule_name << "\":{\n";
    ss << "\"count\" : " << rule_app.second << ",\n";
    ss << "\"rule_app_map\":{\n";
    for (auto itr = rp.begin(); itr != rp.end(); itr++) {
      ss << "\"" << itr->first << "\":" << itr->second << ",";
    }
    ss.seekp(-1, std::ios_base::end);
    ss << "}\n";
    ss << "},";
  }
  ss.seekp(-1, std::ios_base::end);
  ss << "\n},\n";

  ss << "\"rule_set_map\": {\n";
  for (auto it = rule_set_map.begin(); it != rule_set_map.end(); it++) {
    const std::unordered_set<std::string> &rule_set = it->first;
    const RuleApplication &rule_app = it->second;
    const std::unordered_map<std::string, int> &rp = rule_app.first;
    ss << "\"" << rule_index_map[rule_set] << "\":{\n";
    ss << "\"count\" : " << rule_app.second << ",\n";
    ss << "\"rule_app_map\":{\n";
    for (auto itr = rp.begin(); itr != rp.end(); itr++) {
      ss << "\"" << itr->first << "\":" << itr->second << ",";
      ss.seekp(-1, std::ios_base::end);
    }
    ss << "}\n";
    ss << "},";
  }
  ss.seekp(-1, std::ios_base::end);
  ss << "}";
  ss << "}";
  return ss.str();
}
void GenChain::clearChain() {

  chain.clear();
  rule_map.clear();
  rule_set_map.clear();
  return;
}
GenChain GenChain::load(std::string filename) {

  GenChain new_chain;
  // std::cout << "Personally" << std::endl;
  // std::ifstream file(filename);

  // {
  //   boost::archive::text_iarchive ia{file};
  //   ia >> new_chain;
  // }

  // std::cout << "we died" << std::endl;
  return new_chain;
}

// std::ostream &operator<<(std::ostream &os, const GenChain &gc) {

//   for (auto it = gc.chain.begin(); it != gc.chain.end(); it++) {

//     os << it->rule_id;
//     if (it != gc.chain.end() - 1) {
//       os << " -> ";
//     }
//   }
//   os << std::endl;

//   return os;
// }
