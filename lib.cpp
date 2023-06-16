#include <ctype.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "crc32.h"
#include "lib.h"

std::optional<std::set<fs::path>> listDirectoryFiles(const fs::path &dir) {
  std::set<fs::path> paths;
  if (fs::exists(dir) && fs::is_directory(dir)) {
    for (const auto &entry : fs::directory_iterator(dir)) {
      paths.insert(entry.path());
    }
    return paths;
  }
  return {};
}

int digitCount(const std::string &str) {
  int digitCount = 0;
  for (const char &c : str) {
    if (isdigit(c))
      digitCount++;
  }
  return digitCount;
}

ChangeSet dedupeImagePaths(const std::set<fs::path> &input,
                           fs::path outputPath) {
  std::vector<std::pair<fs::path, fs::path>> io;
  fs::path dest = fs::absolute(outputPath);

  // Initialize layer
  std::array<int, 256> layer_count;
  layer_count.fill(1);

  std::map<uint32_t, int> checksums;
  for (const fs::path &f : input) {
    if (auto res = crc32file(f.string())) {
      uint32_t chk = res;
      if (checksums.find(chk) == checksums.end()) {
        std::string filename = f.filename().string();
        auto nameEnd = std::find_if(filename.begin(), filename.end(),
                                    [](char c) { return isdigit(c); });

        std::string layerHeader = filename.substr(0, filename.find(*nameEnd));
        char layer = filename[0];

        // Specify the zero padding in the fmt string
        char fmt[256];
        snprintf(fmt, 255, "%%s%%0%dd%%s", digitCount(f.filename().string()));

        // Create the name
        char buf[256];
        snprintf(buf, 255, fmt, layerHeader.c_str(), layer_count[layer],
                 f.extension().string().c_str());

        // Copy file
        io.push_back({f, dest / buf});

        layer_count[layer]++;
      }
      checksums[chk]++;
    }
  }
  return io;
}

void executeChanges(const ChangeSet &changes, fs::path outputPath) {
  // Create output directory if it doesn't exist
  fs::path dest = fs::absolute(outputPath);
  if (!fs::exists(dest)) {
    fs::create_directory(dest);
  }

  for (const auto &[in, out] : changes) {
    try {
      fs::copy_file(in, out, fs::copy_options::overwrite_existing);
    } catch (fs::filesystem_error &e) {
      std::cout << "Could not copy file: " << e.what() << '\n';
    }
  }
}
