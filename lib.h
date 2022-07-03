#ifndef LIB_H
#define LIB_H

#include <filesystem>
#include <set>
#include <optional>
#include <vector>

namespace fs = std::filesystem;

typedef std::vector<std::pair<fs::path, fs::path>> ChangeSet;

std::optional<std::set<fs::path>> listDirectoryFiles(const fs::path &dir);
int digitCount(const std::string &str);
ChangeSet dedupeImagePaths(const std::set<fs::path> &imagePaths);
void executeChanges(const ChangeSet &changes);

#endif
