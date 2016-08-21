#include "ResponseHandler.h"

#include <string>
#include <iostream>

const CefString& StdInputContentProvider::GetContent()
{
    std::string input;

    std::cout << "Waiting for input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)" << std::endl;

    for (std::string line; std::getline(std::cin, line);) {
        input.append(line + "\n");
    }

    if (input.size() == 0) {
        input = "<!DOCTYPE html>\n";
    }

    return input;
}
