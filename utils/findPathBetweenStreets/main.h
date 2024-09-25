#ifndef FIND_PATH_BETWEEN_STREETS_H
#define FIND_PATH_BETWEEN_STREETS_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>

std::vector<int> findPathBetweenStreets(int startStreet, int endStreet, const std::unordered_map<int, std::vector<int>>& streetConnections);

#endif  // FIND_PATH_BETWEEN_STREETS_H
