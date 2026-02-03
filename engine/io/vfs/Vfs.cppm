module;
#include <cstdint>
#include <string>

export module Rhodo.Io.Vfs;

namespace rhodo::io::vfs {

enum class PathScheme : std::uint8_t {
  Resource,
  Asset,
  Config,
  Save,
  Temp
};
export auto to_string(PathScheme e) -> const char* {
  switch (e) {
    case PathScheme::Resource:
      return "Resource";
    case PathScheme::Asset:
      return "Asset";
    case PathScheme::Config:
      return "Config";
    case PathScheme::Save:
      return "Save";
    case PathScheme::Temp:
      return "Temp";
    default:
      return "unknown";
  }
}

struct Path {};

class VFS final {
 public:
  VFS() = delete;
  VFS(const VFS&) = delete;
  VFS(VFS&&) = delete;
  auto operator=(const VFS&) -> VFS& = delete;
  auto operator=(VFS&&) -> VFS& = delete;

  ~VFS() = default;

  static auto initialize() -> void;

  static auto resolve_path(std::string_view virtual_path) -> std::string;

 private:
};

}  // namespace rhodo::io::vfs
