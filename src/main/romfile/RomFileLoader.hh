#pragma once

#include "RomFile.hh"
#include <optional>
#include <string>

namespace gb4e
{
std::optional<RomFile> LoadRomFile(std::string const & filename);
};