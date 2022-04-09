#include "Graph.h"
#include <queue>

Graph::Graph() {
  boost::uuids::uuid uid = boost::uuids::random_generator()();
  sample_id = boost::uuids::to_string(uid);
  N = 0;
}

Graph::Graph(Graph &&source) noexcept
    : graph(std::move(source.graph)), sample_id(source.sample_id), N(0) {}

Graph::Graph(const Graph &g) {
  this->graph.clear();
  // std::cout << "Copy constructor called" << std::endl;
  sample_id = g.sample_id;
  boost::copy_graph(g.graph, this->graph);
  N = g.N;
}
Graph::~Graph() {}

/* structure modification methods */
void Graph::clear() {
  graph.clear();

  boost::uuids::uuid uid = boost::uuids::random_generator()();
  sample_id = boost::uuids::to_string(uid);
  N = 0;
}

Graph::Vertex Graph::AddVertex(VertexProperties vp) {
  Graph::Vertex v = add_vertex(graph);
  Graph::IndexMap vi = get(boost::vertex_index, graph);
  Graph::VPmap vps = get(vertex_properties, graph);
  vi[v] = N;
  vps[v] = VertexProperties(vp);

  // graph[v].index = N;
  N++;
  // this->prop_map_index[v] = N;
  /* properties(v) = prop; */
  return v;
}

void Graph::RemoveVertex(const Vertex &v) {
  clear_vertex(v, graph);
  remove_vertex(v, graph);
  N--;
}

// void RemoveEdge(const Edge &e) {
//   std::cout << "removing edge" << std::endl;
//   std::cout << e << std::endl;
//   remove_edge(e, graph);
// }
Graph::Edge Graph::AddEdge(const Vertex &v1, const Vertex &v2) {
  Edge addedEdge = add_edge(v1, v2, graph).first;

  /* properties(addedEdge1) = prop_12; */
  /* properties(addedEdge2) = prop_21; */
  return addedEdge;
}
/* selectors and properties */
const Graph::GraphContainer &Graph::getGraph() const { return graph; }

Graph::vertex_range_t Graph::getVertices() const { return vertices(graph); }

Graph::adjacency_vertex_range_t
Graph::getAdjacentVertices(const Vertex &v) const {
  return adjacent_vertices(v, graph);
}

int Graph::getVertexCount() const { return num_vertices(graph); }

int Graph::getVertexDegree(const Vertex &v) const {
  return out_degree(v, graph);
}

VisGraph Graph::toVisGraph() {

  std::unordered_set<std::string> safe_rooms = {"e", "g", "l",  "lf", "lm",
                                                "n", "k", "kf", "km"};
  std::unordered_set<std::string> non_critical_rooms = {"n", "l", "t"};
  VPmap vp = get(vertex_properties, graph);

  VisGraph vis_graph;
  vis_graph.id = sample_id;
  vertex_iter it, end;
  Vertex entrance, goal;
  for (boost::tie(it, end) = boost::vertices(graph); it != end; ++it) {
    VisNode vn;
    vn.id = vp[*it].id;
    vn.index = vp[*it].index;
    vn.abbrev = vp[*it].abbrev;
    vn.label = vp[*it].label;
    vn.mark = vp[*it].mark;

    vis_graph.nodes.push_back(vn);
    if (vp[(*it)].abbrev == "e") {
      entrance = *it;
    }
    if (vp[(*it)].abbrev == "g") {
      goal = *it;
    }
  }

  edge_iter itr, endr;
  for (tie(itr, endr) = boost::edges(graph); itr != endr; ++itr) {
    VisEdge ve;
    ve.from = vp[boost::source(*itr, graph)].id;
    ve.to = vp[boost::target(*itr, graph)].id;
    vis_graph.edges.push_back(ve);
  }
  vis_graph.leniency = Leniency(*this, safe_rooms);
  vis_graph.mission_linearity = MissionLinearity(*this, entrance, goal);
  vis_graph.map_linearity = MapLinearity(*this);
  vis_graph.path_redundancy = PathRedundancy(*this, non_critical_rooms);

  return vis_graph;
}

std::string Graph::toJSON() {
  std::stringstream ss;
  std::unordered_set<std::string> safe_rooms = {"e", "g", "l",  "lf", "lm",
                                                "n", "k", "kf", "km"};
  std::unordered_set<std::string> non_critical_rooms = {"n", "l", "t"};
  VPmap vp = get(vertex_properties, graph);
  ss << "{ \n";

  VisGraph vis_graph;
  vis_graph.id = sample_id;
  ss << "\t \"sample_id\": \"" << sample_id << "\" ,\n";
  Graph::vertex_iter it, end;
  Vertex entrance, goal;
  ss << "\t \"vertices\": [\n";
  for (boost::tie(it, end) = boost::vertices(graph); it != end; ++it) {
    VisNode vn;
    ss << "\t\t{\n";
    ss << "\t\t\t\"id\":\"" << vp[*it].id << "\";\n";
    vn.id = vp[*it].id;
    ss << "\t\t\t\"index\":" << vp[*it].index << ";\n";
    vn.index = vp[*it].index;
    ss << "\t\t\t\"abbrev\":\"" << vp[*it].abbrev << "\";\n";
    vn.abbrev = vp[*it].abbrev;
    ss << "\t\t\t\"label\":\"" << vp[*it].label << "\";\n";
    vn.label = vp[*it].label;
    ss << "\t\t\t\"mark\":\"" << vp[*it].mark << "\";\n";
    vn.mark = vp[*it].mark;
    ss << "\t\t}\n";

    vis_graph.nodes.push_back(vn);
    if (vp[(*it)].abbrev == "e") {
      entrance = *it;
    }
    if (vp[(*it)].abbrev == "g") {
      goal = *it;
    }
  }
  ss << "\t]\n";

  edge_iter itr, endr;
  ss << "\t\"edges\": [\n";
  for (tie(itr, endr) = boost::edges(graph); itr != endr; ++itr) {
    VisEdge ve;
    ss << "\t\t{\n";
    ss << "\t\t\t \"from\": \"" << vp[boost::source(*itr, graph)].id << "\";\n";
    ve.from = vp[boost::source(*itr, graph)].id;
    ss << "\t\t\t \"to\": \"" << vp[boost::target(*itr, graph)].id << "\";\n";
    ve.to = vp[boost::target(*itr, graph)].id;
    ss << "\t\t}\n";
    vis_graph.edges.push_back(ve);
  }
  ss << "]\n";
  vis_graph.leniency = Leniency(*this, safe_rooms);
  ss << "\t\"Leniency\": " << vis_graph.leniency << "\n";
  vis_graph.mission_linearity = MissionLinearity(*this, entrance, goal);
  ss << "\t\"MissionLinearity\": " << vis_graph.mission_linearity << "\n";
  vis_graph.map_linearity = MapLinearity(*this);
  ss << "\t\"MapLinearity\": " << vis_graph.map_linearity << "\n";
  vis_graph.path_redundancy = PathRedundancy(*this, non_critical_rooms);
  ss << "\t\"PathRedundancy\": " << vis_graph.path_redundancy << "\n";
  ss << "}";

  return ss.str();
}
void Graph::saveGraphViz(const char *filename) {
  // std::ofstream file;
  // file.open(filename);
  // VPmap vp = get(vertex_properties, graph);
  // file << "digraph G{ \n";

  // Graph::vertex_iter it, end;
  // for (boost::tie(it, end) = boost::vertices(graph); it != end; ++it) {
  //   file << "\"" << vp[*it].id << "\" [label = \"" << vp[*it].label <<
  //   "\"];\n";
  // }

  // edge_iter itr, endr;
  // for (tie(itr, endr) = boost::edges(graph); itr != endr; ++itr) {
  //   file << "\"" << vp[boost::source(*itr, graph)].id << "\" -> "
  //        << "\"" << vp[boost::target(*itr, graph)].id << "\";\n";
  // }
  // file << "}";

  // file.close();
}

/* operators */
Graph &Graph::operator=(const Graph &rhs) {
  graph = rhs.graph;
  return *this;
}

// std::ostream &operator<<(std::ostream &os, const Graph &graph) {
//   Graph::edge_iter it, end;
//   for (tie(it, end) = boost::edges(graph.graph); it != end; ++it) {
//     // auto vp = get(boost::vertex_index, graph.graph);
//     auto vp2 = get(vertex_properties, graph.graph);
//     // std::cout << vp[boost::source(*it, graph.graph)] << std::endl;
//     // std::cout << vp[u].abbrev << std::endl;
//     os << vp2[boost::source(*it, graph.graph)].label << " -> "
//        << vp2[boost::target(*it, graph.graph)].label;
//     if (std::next(it) != end) {
//       os << std::endl;
//     }
//   }
//   return os;
// }

void Graph::reindex() {

  Graph::vertex_iter it, end;
  int i = 0;
  auto vp = get(vertex_properties, graph);
  for (boost::tie(it, end) = boost::vertices(graph); it != end; ++it) {
    vp[*it].index = i;
    i++;
  }
}

double Graph::Leniency(Graph &g, std::unordered_set<std::string> safe_rooms) {
  double score = 0;
  int total_rooms = g.getVertexCount();
  int safe_rooms_count = 0;
  Graph::vertex_iter it, end;
  auto vp = get(vertex_properties, g.graph);
  for (boost::tie(it, end) = boost::vertices(g.graph); it != end; ++it) {
    if (safe_rooms.find(vp[*it].abbrev) != safe_rooms.end()) {
      safe_rooms_count++;
    }
  }
  score = (double)safe_rooms_count / total_rooms;
  return score;
}
int Graph::num_nodes(Graph &g, Graph::Vertex &start, Graph::Vertex &end) {
  auto vp = get(vertex_properties, g.graph);
  // std::cout << g << std::endl;
  std::unordered_map<int, int> distance;
  std::unordered_map<int, bool> visited;
  std::unordered_map<int, int> previous;
  std::unordered_map<int, vertex_iter> toVert;
  Graph::vertex_iter it, endr;
  std::priority_queue<VertexProperties, std::vector<VertexProperties>,
                      CompareDistance>
      pq;
  int i = 0;
  Graph::Vertex e, go;
  for (boost::tie(it, endr) = boost::vertices(g.graph); it != endr; ++it) {

    vp[*it].index = i;
    // std::cout << "vp_index: " << vp[*it].index << std::endl;
    // std::cout << "vp_label: " << vp[*it].label << std::endl;
    i++;
    distance[vp[*it].index] = INT_MAX - 1;
    vp[*it].distance = INT_MAX - 1;
    visited[vp[*it].index] = false;
    previous[vp[*it].index] = -1;
    toVert[vp[*it].index] = it;
    if (vp[*it].abbrev == vp[start].abbrev) {
      e = start;
      distance[vp[*it].index] = 0;
      vp[*it].distance = 0;
      // std::cout << "vp_index: " << vp[*it].index << std::endl;
      // std::cout << "vp_label: " << vp[*it].label << std::endl;
      // std::cout << "vp_distance: " << vp[*it].distance << std::endl;
      // std::cout << "vp_abbrev: " << vp[*it].abbrev << std::endl;
    }
    if (vp[*it].abbrev == vp[end].abbrev) {
      go = end;
    }
  }

  pq.push(vp[e]);

  // std::cout << "assert(l);" << std::endl;
  while (pq.size() != 0) {
    // std::cout << "outer loop: " << pq.size() << std::endl;
    VertexProperties u = pq.top();
    // std::cout << "considering: " << u.label << " u.index: " << u.index
    //           << std::endl;
    int v_degree = g.getVertexDegree(*toVert[u.index]);
    // std::cout << "degree: " << v_degree << std::endl;
    pq.pop();
    visited[u.index] = true;
    // std::cout << "distance[u.index]: " << distance[u.index] << std::endl;
    // std::cout << "u_index: " << u.index << " u_label: " << u.label <<
    // std::endl; std::cout << "u_distance: " << u.distance << std::endl;
    // std::cout << "u_abbrev: " << u.abbrev << std::endl;
    if (u.distance > distance[u.index]) {
      // std::cout << "what the fuck" << std::endl;
      VertexProperties u = pq.top();
      // std::cout << "influence    u_index: " << u.index
      //           << " u_label: " << u.label << std::endl;
      continue;
    }
    adjacency_iter ai, a_end;
    // std::cout << "u.label, v_degree: " << u.label << ": " << v_degree
    //           << std::endl;
    boost::tie(ai, a_end) = adjacent_vertices(*toVert[u.index], g.graph);
    // std::cout << vp[e].distance << std::endl;
    for (; ai != a_end; ai++) {
      // std::cout << "--------------------------------" << std::endl;
      // std::cout << "u_index: " << u.index << std::endl;
      // std::cout << "u_label: " << u.label << std::endl;
      int new_dist = distance[u.index] + 1;

      if (visited[vp[*ai].index]) {
        // std::cout << "we are skippnig" << std::endl;
        continue;
      }

      // std::cout << "--------------------------------" << std::endl;
      // std::cout << "new_dist: " << new_dist << std::endl;
      // std::cout << "--------------------------------" << std::endl;
      // std::cout << "distance[vp[*ai].index]: " <<
      // distance[vp[*ai].index]
      //           << std::endl;
      // std::cout << "--------------------------------" << std::endl;
      // std::cout << "vp[*ai].distance: " << vp[*ai].distance <<
      // std::endl; std::cout << "--------------------------------" <<
      // std::endl; std::cout << "kl: u.index: " << u.index << " ,
      // u.label:" << u.label
      //           << std::endl;
      // std::cout << "previous: " << previous[u.index] << std::endl;
      // std::cout << "--------------------------------" << std::endl;
      if (new_dist < distance[vp[*ai].index]) {
        // std::cout << "adding: " << vp[*ai].label << std::endl;
        // std::cout << "adding: " << vp[*ai].label << " index: " <<
        // vp[*ai].index
        // << std::endl;
        distance[vp[*ai].index] = new_dist;
        // if (vp[*ai].index == u.index) {
        // std::cout << "previous: " << previous[vp[*ai].index] << std::endl;
        // std::cout << "previou 2 2s: " << vp[*ai].index << std::endl;
        // }
        previous[vp[*ai].index] = u.index;
        // std::cout << "previous: " << previous[vp[*ai].index] << std::endl;
        vp[*ai].distance = new_dist;
        pq.push(vp[*ai]);
        // std::cout << "in loop: " << pq.size() << std::endl;
        // std::cout << pq.size() << std::endl;
        // std::cout << "u.index: " << u.index << std::endl;
      }
    }
  }

  int v = -1;
  std::vector<int> path_reverse;
  // std::cout << "end's previous: " << previous[vp[go].index] << std::endl;
  path_reverse.push_back(previous[vp[go].index]);
  v = vp[go].index;
  // std::cout << "end's previous: " << previous[vp[go].index] << std::endl;
  // std::cout << v << std::endl;
  // std::cout << "end's previous: " << previous[vp[go].index] << std::endl;
  while (v != -1) {
    v = previous[v];
    path_reverse.push_back(v);
  }
  return path_reverse.size();
  // pq.push(entrance); std::cout << pq.size() << std::endl;
}
double Graph::MissionLinearity(Graph &g, Graph::Vertex start,
                               Graph::Vertex end) {

  int num_nodes_shortest_path = num_nodes(g, start, end);
  int total_nodes = g.getVertexCount();
  double score = (double)num_nodes_shortest_path / total_nodes;

  return score;
}
double Graph::MapLinearity(Graph &g) {
  double score = 0;
  int total_rooms_with_exits = 0;
  int single_exits = 0;
  int double_exits = 0;
  Graph::vertex_iter it, end;
  for (boost::tie(it, end) = boost::vertices(g.graph); it != end; ++it) {
    int v_degree = g.getVertexDegree(*it);
    if (v_degree > 0) {
      total_rooms_with_exits++;
    }
    if (v_degree == 1) {
      single_exits++;
    }
    if (v_degree == 2) {
      double_exits++;
    }
  }
  score = (single_exits + (0.5 * double_exits)) / total_rooms_with_exits;
  return score;
}
double
Graph::PathRedundancy(Graph &g,
                      std::unordered_set<std::string> non_critical_rooms) {
  double score = 0;
  int total_rooms = g.getVertexCount();
  int non_critical_count = 0;
  Graph::vertex_iter it, end;
  auto vp = get(vertex_properties, g.graph);
  for (boost::tie(it, end) = boost::vertices(g.graph); it != end; ++it) {
    if (non_critical_rooms.find(vp[*it].abbrev) != non_critical_rooms.end()) {
      non_critical_count++;
    }
  }
  score = (double)non_critical_count / total_rooms;
  return score;
}
