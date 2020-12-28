#include "SlurpFile.hh"

#include "logging/Logger.hh"

auto const logger = Logger::Create("SlurpFile");

namespace gb4e
{
std::optional<SizedArray> SlurpFile(std::string const & filename)
{
    FILE * file;
    errno_t err = fopen_s(&file, filename.c_str(), "rb");
    if (err != 0) {
        logger->Errorf("Failed to open filename=%s, err=%d", filename.c_str(), err);
        return {};
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    u8 * rawFile = new u8[fileSize];
    fread(rawFile, 1, fileSize, file);

    return SizedArray(fileSize, std::unique_ptr<u8[]>(rawFile));
}
};