#include "radix.hpp"
#include <cassert>
#include <iostream>

int main() {
  kit::RadixTree<int> t;
  t.insert("test", 1);
  t.insert("team", 2);
  t.insert("toast", 3);
  t.insert("tea", 4);
  assert(t.find("team").value() == 2);
  assert(t.find("tea").value() == 4);
  assert(!t.find("te"));
  auto pref = t.prefix("te");
  assert(pref.size() == 3);
  std::cout << "radix ok prefix=" << pref.size() << "\n";
}
