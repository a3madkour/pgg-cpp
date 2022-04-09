#ifndef VISNODE_H_
#define VISNODE_H_
#include <string>
struct VisNode {
  std::string id;
  std::string index;
  std::string abbrev;
  std::string label;
  std::string mark;

  static std::string getStringFromInstance(const VisNode &instance) {
    return instance.id;
  }
};
#endif // VISNODE_H_
