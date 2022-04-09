#ifndef GRAPH_H_
#define GRAPH_H_

#include "VertexProperties.h"
#include "VisGraph.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_utility.hpp>
#include <sstream>

enum vertex_properties_t { vertex_properties };
enum edge_properties_t { edge_properties };
namespace boost {
BOOST_INSTALL_PROPERTY(vertex, properties);
BOOST_INSTALL_PROPERTY(edge, properties);

} // namespace boost
// struct EdgeProperties {

//   template <class Archive> void serialize(Archive &ar, unsigned version) {}
// };
/* the graph base class template */
class Graph {
public:
  /* an adjacency_list like we need it */
  typedef boost::adjacency_list<
      boost::setS,           // disallow parallel edges
      boost::listS,          // vertex container
      boost::bidirectionalS, // directed graph
      boost::property<boost::vertex_index_t, int,
                      boost::property<vertex_properties_t, VertexProperties>>>
      // EdgeProperties>

      GraphContainer;
  typedef
      typename boost::graph_traits<GraphContainer>::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<GraphContainer>::edge_descriptor Edge;
  typedef std::pair<Edge, Edge> EdgePair;
  typedef boost::property_map<GraphContainer, boost::vertex_index_t>::type
      IndexMap;
  typedef boost::property_map<GraphContainer, vertex_properties_t>::type VPmap;
  typedef
      typename boost::graph_traits<GraphContainer>::vertex_iterator vertex_iter;
  typedef typename boost::graph_traits<GraphContainer>::edge_iterator edge_iter;
  typedef typename boost::graph_traits<GraphContainer>::adjacency_iterator
      adjacency_iter;
  typedef typename boost::graph_traits<GraphContainer>::out_edge_iterator
      out_edge_iter;
  typedef
      typename boost::graph_traits<GraphContainer>::degree_size_type degree_t;
  typedef std::pair<adjacency_iter, adjacency_iter> adjacency_vertex_range_t;
  typedef std::pair<out_edge_iter, out_edge_iter> out_edge_range_t;
  typedef std::pair<vertex_iter, vertex_iter> vertex_range_t;
  typedef std::pair<edge_iter, edge_iter> edge_range_t;

  GraphContainer graph;
  std::string sample_id;
  int N;
  /* a bunch of graph-specific typedefs */
  Graph();
  Graph(Graph &&source) noexcept;
  Graph(const Graph &g);
  virtual ~Graph();
  void clear();
  Vertex AddVertex(VertexProperties vp);
  void RemoveVertex(const Vertex &v);
  Edge AddEdge(const Vertex &v1, const Vertex &v2);
  /* selectors and properties */
  const GraphContainer &getGraph() const;
  vertex_range_t getVertices() const;
  adjacency_vertex_range_t getAdjacentVertices(const Vertex &v) const;

  int getVertexCount() const;

  int getVertexDegree(const Vertex &v) const;
  void reindex();

  std::string toJSON();
  VisGraph toVisGraph();
  void saveGraphViz(const char *filename);
  /* operators */
  Graph &operator=(const Graph &rhs);
  // friend std::ostream &operator<<(std::ostream &os, const Graph &graph);
  static double Leniency(Graph &g, std::unordered_set<std::string> safe_rooms);
  static double MissionLinearity(Graph &g, Graph::Vertex start,
                                 Graph::Vertex end);
  static int num_nodes(Graph &g, Graph::Vertex &start, Graph::Vertex &end);
  static double MapLinearity(Graph &g);
  static double
  PathRedundancy(Graph &g, std::unordered_set<std::string> non_critical_rooms);
};

#endif // GRAPH_H_
