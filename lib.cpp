#include <vector>
#include <map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <anitk/lib.h>

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
std::optional<GLuint> LoadTextureFromFile(const std::string &filename, int* out_width, int* out_height) {
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

// void dedupe(fs::path folder, char layer) {
//   auto files = getLayerFiles(folder, layer);
//   std::set<std::string> checksums;
//   fs::path dest("output");
//   char buf[256];
//   int n = 1;
//   for (const auto& f : files) {
//     auto chk = md5sum(f);
//     if (checksums.find(chk) != checksums.end()) {
//       sprintf(buf, "%c_%d", layer, n);
//       copy_file(f, dest / buf);

//       checksums.insert(chk);
//       n++;
//     }
//   }
// }
