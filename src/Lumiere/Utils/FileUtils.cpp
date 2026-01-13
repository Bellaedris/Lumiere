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
} // mgl::utils