#include "StdInput.h"

#include <string>
#include <iostream>

namespace cefpdf {
namespace job {

const std::string& StdInput::GetContent()
{
    if (m_content.empty()) {
        std::string input;

        std::cout << "Waiting for input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)" << std::endl;

        for (std::string line; std::getline(std::cin, line);) {
            input.append(line + "\n");
        }

        if (input.size() == 0) {
            input = "<!DOCTYPE html>\n";
        }

        m_content = input;
    }

    return m_content;
}

} // namespace job
} // namespace cefpdf
