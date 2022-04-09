#include "VertexProperties.h"

VertexProperties::VertexProperties(VertexProperties &&source) noexcept
    : index(source.index), abbrev(std::move(source.abbrev)),
      label(std::move(source.label)), id(std::move(source.id)),
      mark(std::move(source.mark)), distance(std::move(source.distance)) {

  // std::cout << "VP move contructor" << std::endl;
}
VertexProperties::VertexProperties() {

  boost::uuids::uuid uid = boost::uuids::random_generator()();
  index = 0;
  abbrev = "";
  label = "";
  id = boost::uuids::to_string(uid);
  mark = "";
  distance = INT_MAX;
  // std::cout << "VP default contructor" << std::endl;
}

VertexProperties &VertexProperties::operator=(const VertexProperties &rhs) {
  index = rhs.index;
  abbrev = rhs.abbrev;
  label = rhs.label;
  id = rhs.id;
  mark = rhs.mark;
  distance = rhs.distance;
  return *this;
}
VertexProperties::VertexProperties(const VertexProperties &vep)
    : index(vep.index), abbrev(vep.abbrev), label(vep.label), id(vep.id),
      mark(vep.mark), distance(vep.distance) {}
