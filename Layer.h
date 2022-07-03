#include <filesystem>
#include <set>
#include <map>

#include "crc32.h"

namespace fs = std::filesystem;

class Layer {
  std::set<fs::path> imagePaths;
  std::string name;
public:

  Layer() {
  }

  Layer(std::string _name) : name(_name) {
  }

  Layer(std::string _name, std::set<fs::path>_images) : name(_name), imagePaths(_images) {
  }
  
  void addImage(fs::path imagePath) {
    imagePaths.insert(imagePath);
  }

  void groupDuplicates() {
    std::map<uint32_t, std::set<fs::path>> groupsByChecksum;
    
    for (const fs::path &imagePath : imagePaths) {
      if (auto res = crc32file(imagePath.string())) {
        uint32_t chk = res.value().first;
        groupsByChecksum[chk].insert(imagePath);
      }
    }

    std::vector<std::vector<fs::path>> groupBySequence;
    for (const auto &[k, v] : groupsByChecksum) {
      groupBySequence.emplace_back(v);
    }

  }
};
