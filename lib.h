#ifndef LIB_H
#define LIB_H

#include <filesystem>
#include <set>
#include <optional>
#include <vector>

#include <GL/gl.h>
#include <GL/glext.h>

namespace fs = std::filesystem;

std::optional<std::set<fs::path>> ListDirectory(const fs::path &p, bool sorted = false);
std::optional<GLuint> LoadTextureFromFile(const std::string &filename, int *out_width, int *out_height);
int digitCount(const std::string &str);
void dedupe(fs::path, fs::path);
std::vector<std::pair<fs::path, fs::path>> dedupeImagePaths(std::set<fs::path>);
void executeChanges(std::vector<std::pair<fs::path, fs::path>> changes);

#endif
