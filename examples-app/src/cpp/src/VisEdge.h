#ifndef VISEDGE_H_
#define VISEDGE_H_
#include <string>

struct VisEdge {

  std::string from;
  std::string to;

  static std::string getStringFromInstance(const VisEdge &instance) {
    return instance.from + instance.to;
  }
};
#endif // VISEDGE_H_
