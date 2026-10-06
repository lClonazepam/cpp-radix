# cpp-radix

Header-only compressed radix tree. Exact find plus prefix scan, useful for routing tables, autocomplete, and config keys.

```cpp
kit::RadixTree<int> routes;
routes.insert("/api/users", 1);
auto hits = routes.prefix("/api");
```

MIT
