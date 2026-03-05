//
// Created by Bellaedris on 12/12/2025.
//

#include <fstream>
#include <sstream>
#include "FileUtils.h"

namespace lum::utils
{
std::optional<std::string> FileUtils::read_file(const char *filename)
{
    std::ifstream file(filename, std::ifstream::in);
    if (file)
    {
        //read the shader file
        std::stringbuf s;
        file.get(s, 0);
        file.close();

        return s.str();
    }

    return {};
}

std::string FileUtils::PathToName(const std::string &path)
{
    size_t name = path.find_last_of('/');
    size_t extension = path.find_last_of('.');

    return path.substr(name + 1, extension - 1);
}
} // mgl::utils