#pragma once
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kit {

template <typename Value>
class RadixTree {
 public:
  void insert(std::string_view key, Value value) { insert_at(root_, key, std::move(value)); }

  std::optional<Value> find(std::string_view key) const {
    const Node* n = &root_;
    while (!key.empty()) {
      auto it = n->children.find(key.front());
      if (it == n->children.end()) return std::nullopt;
      const auto& edge = it->second->edge;
      if (key.size() < edge.size() || key.substr(0, edge.size()) != edge) return std::nullopt;
      key.remove_prefix(edge.size());
      n = it->second.get();
    }
    if (!n->has) return std::nullopt;
    return n->value;
  }

  std::vector<std::pair<std::string, Value>> prefix(std::string_view key) const {
    const Node* n = &root_;
    std::string walked;
    while (!key.empty()) {
      auto it = n->children.find(key.front());
      if (it == n->children.end()) return {};
      const auto& edge = it->second->edge;
      const auto shared = common(edge, key);
      if (shared == 0) return {};
      if (shared < edge.size()) {
        if (shared != key.size()) return {};
        walked.append(edge);
        n = it->second.get();
        key = {};
        break;
      }
      walked.append(edge);
      key.remove_prefix(edge.size());
      n = it->second.get();
    }
    std::vector<std::pair<std::string, Value>> out;
    collect(n, walked, out);
    return out;
  }

 private:
  struct Node {
    std::string edge;
    bool has = false;
    Value value{};
    std::unordered_map<char, std::unique_ptr<Node>> children;
  };
  static std::size_t common(std::string_view a, std::string_view b) {
    std::size_t i = 0;
    while (i < a.size() && i < b.size() && a[i] == b[i]) ++i;
    return i;
  }
  void insert_at(Node& node, std::string_view key, Value value) {
    if (key.empty()) { node.has = true; node.value = std::move(value); return; }
    auto it = node.children.find(key.front());
    if (it == node.children.end()) {
      auto child = std::make_unique<Node>();
      child->edge = std::string(key);
      child->has = true;
      child->value = std::move(value);
      node.children.emplace(child->edge.front(), std::move(child));
      return;
    }
    Node& child = *it->second;
    const auto shared = common(child.edge, key);
    if (shared == child.edge.size()) { insert_at(child, key.substr(shared), std::move(value)); return; }
    auto split = std::make_unique<Node>();
    split->edge = child.edge.substr(0, shared);
    child.edge = child.edge.substr(shared);
    auto rest = std::move(it->second);
    node.children.erase(it);
    if (shared == key.size()) { split->has = true; split->value = std::move(value); }
    else {
      auto leaf = std::make_unique<Node>();
      leaf->edge = std::string(key.substr(shared));
      leaf->has = true;
      leaf->value = std::move(value);
      split->children.emplace(leaf->edge.front(), std::move(leaf));
    }
    split->children.emplace(rest->edge.front(), std::move(rest));
    node.children.emplace(split->edge.front(), std::move(split));
  }
  void collect(const Node* n, const std::string& prefix, std::vector<std::pair<std::string, Value>>& out) const {
    if (n->has) out.emplace_back(prefix, n->value);
    for (const auto& [_, child] : n->children) collect(child.get(), prefix + child->edge, out);
  }
  Node root_;
};

}  // namespace kit
