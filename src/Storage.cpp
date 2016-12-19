#include "Storage.h"
#include "Common.h"

#include <fstream>
#include <functional>
#include <random>
#include <algorithm>
#include <cstdlib>

namespace cefpdf {

Storage::Storage() {
    m_temp = getTempDirectory();
}

std::string Storage::Reserve()
{
    std::string letters = "9876543210ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(letters.begin(), letters.end(), g);
    std::string path = m_temp + "cef_" + letters + ".pdf";

    std::ifstream file;
    file.open(path);
    bool isGood = file.good();
    file.close();

    return isGood ? Reserve() : path;
}

std::string Storage::Load(const std::string& path, bool remove)
{
    std::string content;
    std::ifstream output;

    output.open(path, std::ifstream::binary);
    if (output.good()) {
        content.assign((std::istreambuf_iterator<char>(output)), std::istreambuf_iterator<char>());
        output.close();
        if (remove) {
            Delete(path);
        }

        return content;
    }

    throw "Cannot open file: \"" + path + "\"";
}

bool Storage::Delete(const std::string& path)
{
    return 0 == std::remove(path.c_str());
}

} // namespace cefpdf
