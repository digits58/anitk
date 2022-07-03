#include <ctype.h>

#include <algorithm>
#include <array>
#include <vector>
#include <map>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "lib.h"
#include "crc32.h"

std::optional<std::set<fs::path>> ListDirectory(const fs::path &p, bool sorted) {
  std::set<fs::path> paths;
  if (fs::exists(p) && fs::is_directory(p)) {
    for (const auto& entry : fs::directory_iterator(p)) {
      paths.insert(entry.path());
    }
    return paths;
  }
  return std::nullopt;
}

// Simple helper function to load an image into a OpenGL texture with common settings
std::optional<GLuint> LoadTextureFromFile(const std::string &filename, int* out_width = nullptr, int* out_height = nullptr) {
  // Load from file
  int image_width = 0;
  int image_height = 0;
  unsigned char* image_data = stbi_load(filename.c_str(), &image_width, &image_height, NULL, 4);
  if (image_data == NULL)
    return std::nullopt;

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  *out_width = image_width;
  *out_height = image_height;

  return {image_texture};
}

int digitCount(const std::string &str) {
  int digitCount = 0;
  for (const char &c : str) {
    if (isdigit(c)) digitCount++;
  }
  return digitCount;
}

std::vector<std::pair<fs::path, fs::path>> dedupeImagePaths(std::set<fs::path> input) {
  std::vector<std::pair<fs::path, fs::path>> io;
  fs::path dest = fs::absolute("output");
  
  // Initialize layer
  std::array<int,256> layer_count;
  layer_count.fill(1);

  std::map<uint32_t, int> checksums;
  for (const fs::path &f : input) {
    if (auto res = crc32file(f.string())) {
      uint32_t chk = res.value().first; 
      if (checksums.find(chk) == checksums.end()) {
        std::string filename = f.filename().string();
        auto nameEnd = std::find_if(filename.begin(), filename.end(), [](char c) { return isdigit(c); });

        std::string layerHeader = filename.substr(0, filename.find(*nameEnd));
        char layer = filename[0];

        // Specify the zero padding in the fmt string
        char fmt[256];
        snprintf(fmt, 255, "%%s%%0%dd%%s", digitCount(f.filename().string()));

        // Create the name
        char buf[256];
        snprintf(buf, 255, fmt, layerHeader.c_str(), layer_count[layer], f.extension().c_str());

        // Copy file
        io.push_back({f, dest / buf});

        layer_count[layer]++;
      }
      checksums[chk]++;
    }
  }
  return io;
}

void executeChanges(std::vector<std::pair<fs::path, fs::path>> changes) {
  // Create output directory if it doesn't exist
  fs::path dest = fs::absolute(fs::path("output"));
  if (!fs::exists(dest)) fs::create_directory(dest);

  for (const auto &[in, out] : changes) {
    try {
      fs::copy_file(in, out, fs::copy_options::overwrite_existing);
    } catch (fs::filesystem_error &e) {
      std::cout << "Could not copy file: " << e.what() << '\n';
    }
  }
}

void dedupe(fs::path inputFolder, fs::path outputFolder) {
  // Fetch the frames in the folder
  std::set<fs::path> frames;
  for (const auto& f : fs::directory_iterator(inputFolder)) {
    if (f.is_regular_file()) {
      frames.insert(f.path());
    }
  }

  // Create output directory if it doesn't exist
  fs::path dest = fs::absolute(outputFolder);
  std::cout << dest << std::endl;
  if (!fs::exists(dest)) fs::create_directory(dest);

  // Initialize layer
  std::array<int,256> layer_count;
  layer_count.fill(1);

  std::map<uint32_t, int> checksums;
  for (const auto& f : frames) {
    if (auto res = crc32file(f.string())) {
      uint32_t chk = res.value().first; 
      if (checksums.find(chk) == checksums.end()) {
        std::string filename = f.filename().string();
        auto nameEnd = std::find_if(filename.begin(), filename.end(), [](char c) { return isdigit(c); });

        std::string layerHeader = filename.substr(0, filename.find(*nameEnd));
        char layer = filename[0];

        // Specify the zero padding in the fmt string
        char fmt[256];
        snprintf(fmt, 255, "%%s%%0%dd%%s", digitCount(f.filename().string()));

        // Create the name
        char buf[256];
        snprintf(buf, 255, fmt, layerHeader.c_str(), layer_count[layer], f.extension().c_str());

        // Copy file
        try {
          fs::path d = dest / buf;
          fs::copy_file(f, d, fs::copy_options::overwrite_existing);
        } catch (fs::filesystem_error &e) {
          std::cout << "Could not copy file: " << e.what() << '\n';
        }

        layer_count[layer]++;
      }
      checksums[chk]++;
    }
  }
}
