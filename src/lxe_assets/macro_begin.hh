#define LXE_ASSETS_DATA(name) \
  auto name() -> std::span<const char>;

#define LXE_ASSETS_DATA_IMPL(name, path) \
  auto name() -> std::span<const char> \
  { \
    const auto d = b::embed<path>(); \
    return {reinterpret_cast<const char*>(d.data()), d.size()}; \
  }
