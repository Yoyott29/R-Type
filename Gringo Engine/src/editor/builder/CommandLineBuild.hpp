#pragma once

#include <string>
#include <vector>

// Builds a project without opening the editor. The arguments come after "--build": exactly one,
// the .gringo file. Returns 0, or 84 on a usage error or a failed build
int buildFromCommandLine(const std::vector<std::string> &arguments);
