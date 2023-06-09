#include <filesystem>
#include <map>
#include <set>
#include <vector>

#include "crc32.h"

namespace fs = std::filesystem;

class CelFolder {

public:
  fs::path folderPath;
  std::set<fs::path> imagePaths;

  std::map<std::string, std::set<fs::path>> layers;
  std::map<std::string, std::set<fs::path>> dedupe;

  CelFolder() {}

  CelFolder(fs::path path) : folderPath(path) { setPath(path); }

  void setPath(fs::path path) {
    folderPath = path;
    if (!fs::exists(folderPath))
      throw;
    if (!fs::is_directory(folderPath))
      throw;
    imagePaths = list();
    layers.clear();
    dedupe.clear();
    getLayers();
    // dedupeLayers();
  }

  std::set<fs::path> list() {
    std::set<fs::path> paths;
    for (const fs::directory_entry &entry :
         fs::directory_iterator(folderPath)) {
      paths.insert(entry.path());
    }
    return paths;
  }

  void getLayers() {
    for (const fs::directory_entry &entry :
         fs::directory_iterator(folderPath)) {
      if (fs::is_regular_file(entry)) {
        std::string header = entry.path().filename().string().substr(0, 1);
        layers[header].insert(entry.path());
      }
    }
  }

  void dedupeLayers() {
    for (auto &[k, v] : layers) {
      dedupe[k] = dedupeLayer(v);
    }
  }

  std::set<fs::path> dedupeLayer(std::set<fs::path> layer) {
    std::set<fs::path> result;
    std::map<uint32_t, bool> dupe;

    for (const fs::path &imagePath : layer) {
      if (auto res = crc32file(imagePath.string())) {
        uint32_t chk = res;

        if (!dupe[chk]) {
          result.insert(imagePath);
          dupe[chk] = true;
        }
      }
    }
    return result;
  }
};
