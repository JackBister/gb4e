#include "RomFileLoader.hh"

#include <cstdio>

#include "SlurpFile.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("RomFileLoader");

std::optional<gb4e::RomFile> gb4e::LoadRomFile(std::string const & filename)
{
    std::optional<SizedArray> romFile = SlurpFile(filename);
    if (!romFile.has_value()) {
        logger->Errorf("Failed to load ROM file with filename=%s", filename.c_str());
        return {};
    }
    return RomFile::Create(romFile.value().size, std::move(romFile.value().arr));
}