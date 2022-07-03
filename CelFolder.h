#include <filesystem>
#include <set>
#include <map>
#include <vector>

#include "crc32.h"

namespace fs = std::filesystem;

class CelFolder {

public:

  fs::path folderPath;
  std::set<fs::path> imagePaths;
  

  std::map<std::string, std::set<fs::path>> layers;
  std::map<std::string, std::set<fs::path>> dedupe;

  std::map<uint32_t, fs::path> instanceHeads;
  std::map<fs::path, uint32_t> instanceChecksums;
  std::map<uint32_t, std::set<fs::path>> duplicates;

  CelFolder(fs::path path) : folderPath(path) {
    if (!fs::exists(folderPath)) throw;
    if (!fs::is_directory(folderPath)) throw;
    imagePaths = list();
    getLayers();
    findDuplicates();
    dedupeLayers();
  }

  std::set<fs::path> list() {
    std::set<fs::path> paths;
    for (const fs::directory_entry &entry : fs::directory_iterator(folderPath)) {
      paths.insert(entry.path());
    }
    return paths;
  }

  void findDuplicates() {
    for (const fs::directory_entry &entry : fs::directory_iterator(folderPath)) {
      if(fs::is_regular_file(entry)) {
        auto imagePath = entry.path();
        if (auto res = crc32file(imagePath.string())) {
          uint32_t chk = res.value().first;

          if (instanceHeads.count(chk) == 0) {
            instanceHeads[chk] = imagePath;
          }
          instanceChecksums[imagePath] = chk;
          duplicates[chk].insert(imagePath);
        }
      }
    }
  }

  void getLayers() {
    for (const fs::directory_entry &entry : fs::directory_iterator(folderPath)) {
      if(fs::is_regular_file(entry)) {
          std::string header = entry.path().filename().string().substr(0,1);
        layers[header].insert(entry.path());
      }
    }
  }

  void dedupeLayers() {
    for (auto &[k,v] : layers) {
      dedupe[k] = dedupeLayer(v);
    }
  }

  std::set<fs::path> dedupeLayer(std::set<fs::path> layer) {
    std::set<fs::path> result;
    std::map<uint32_t, bool> dupe;

    for (const fs::path &imagePath : layer) {
      if (auto res = crc32file(imagePath.string())) {
        uint32_t chk = res.value().first;

        if (!dupe[chk]) {
          result.insert(imagePath);
          dupe[chk] = true;
        }
      }
    }

    return result;
  }
};
