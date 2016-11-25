#include "Storage.h"

#include <fstream>
#include <functional>
#include <algorithm>

namespace cefpdf {

std::string Storage::Reserve()
{
    std::ifstream file;
/*
    auto tt = std::chrono::system_clock::now().time_since_epoch();
    auto ts = std::chrono::duration_cast<std::chrono::nanoseconds>(tt).count();
*/
    std::string letters = "9876543210ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_shuffle(letters.begin(), letters.end());
    std::string path = m_root + "/" + letters + ".pdf";

    file.open(path);
    bool isGood = file.good();
    file.close();

    if (isGood) {
        return Reserve();
    }

    return path;
}

std::string Storage::Load(const std::string& path)
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

bool Storage::Delete(const std::string& path)
{
    return 0 == std::remove(path.c_str());
}

} // namespace cefpdf
