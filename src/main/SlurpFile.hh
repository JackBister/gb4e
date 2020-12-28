#pragma once

#include <optional>
#include <string>

#include "Common.hh"

namespace gb4e
{
std::optional<SizedArray> SlurpFile(std::string const & filename);
};
