#ifndef DRAW_LINE_FOLLOWING_STREET_TOPOLOGY_H
#define DRAW_LINE_FOLLOWING_STREET_TOPOLOGY_H

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include "../../common.h"

void drawLineFollowingStreetTopology(const SelectedPoint& point1, const SelectedPoint& point2, std::unordered_map<int, std::vector<int>> streetConnections);

#endif  // DRAW_LINE_FOLLOWING_STREET_TOPOLOGY_H
