#ifndef LIB_H
#define LIB_H

#include <filesystem>
#include <set>
#include <optional>

#include <GL/gl.h>
#include <GL/glext.h>

namespace fs = std::filesystem;

std::optional<std::set<fs::path>> ListDirectory(const fs::path &p, bool sorted = false);
std::optional<GLuint> LoadTextureFromFile(const std::string &filename, int *out_width, int *out_height);
void dedupe(fs::path folder);

#endif
