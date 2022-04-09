#ifndef VERTEXPROPERTIES_H_
#define VERTEXPROPERTIES_H_

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>
#include <string>

struct VertexProperties {
  int index;
  std::string abbrev;
  std::string label;
  std::string id;
  std::string mark;
  int distance;
  VertexProperties(VertexProperties &&source) noexcept;
  VertexProperties();
  VertexProperties &operator=(const VertexProperties &rhs);
  VertexProperties(const VertexProperties &vep);
};
struct CompareDistance {
  bool operator()(VertexProperties const &vp1, VertexProperties const &vp2) {
    // return "true" if "p1" is ordered
    // before "p2", for example:
    return vp1.distance < vp2.distance;
  }
};
#endif // VERTEXPROPERTIES_H_
