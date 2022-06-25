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

void dedupe(fs::path folder) {
  std::set<fs::path> frames;
  for (const auto& f : fs::directory_iterator(folder)) {
    if (f.is_regular_file()) {
      frames.insert(f.path());
    }
  }

  fs::path dest = fs::absolute("output");
  std::cout << dest << std::endl;
  if (!fs::exists(dest)) fs::create_directory(dest);

  std::array<int,256> layer_count;
  layer_count.fill(1);

  std::map<uint32_t, int> checksums;
  for (const auto& f : frames) {
    std::cout << f << std::endl;
    if (auto res = crc32file(f.string())) {
      uint32_t chk = res.value().first; 
      std::cout << std::hex << chk << std::endl;
      if (checksums.find(chk) == checksums.end()) {
        char layer = f.filename().string()[0];
        char buf[256];
        snprintf(buf, 256, "%c%04d%s", layer, layer_count[layer], f.extension().c_str());
        std::cout << buf << std::endl;
        try {
          fs::path d = dest / buf;
          std::cout << d << std::endl;
          fs::copy_file(fs::absolute(f), d, fs::copy_options::overwrite_existing);
        } catch (fs::filesystem_error &e) {
          std::cout << "Could not copy file: " << e.what() << '\n';
        }

        layer_count[layer]++;
      }
      checksums[chk]++;
      std::cout << std::endl;
    }
  }
  for (auto const &[k, v] : checksums) {
    std::cout<< k << ' ' << v << std::endl;
  }
}
