#include "Storage.h"

#include <fstream>
#include <functional>

namespace cefpdf {

std::string Storage::Reserve(std::string data)
{
    //auto hash = std::hash<std::string>(data);
    return "";
}

std::string Storage::Load(std::string path)
{
    std::string content;
    std::ifstream output;

    output.open(path, std::ifstream::binary);
    if (output.good()) {
        content.assign((std::istreambuf_iterator<char>(output)), std::istreambuf_iterator<char>());
        output.close();
        return content;
    }

    throw "Cannot open file: \"" + path + "\"";
}

} // namespace cefpdf
