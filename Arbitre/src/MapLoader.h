#pragma once

#include <vector>
#include <utility>
#include "../../Commun/interface_gui.h"

using Regions = std::vector< std::vector<std::pair<unsigned int, unsigned int>> >;

void LoadDefaultMap(Regions &regions);
SRegions* ConvertMap(Regions &regions);
void DeleteMap(SRegions *regions);
