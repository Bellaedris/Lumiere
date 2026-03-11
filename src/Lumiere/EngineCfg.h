//
// Created by belle on 11/03/2026.
//

#pragma once
#include <filesystem>

namespace lum::cfg
{
    inline std::filesystem::path EXECUTABLE_DIR;

    inline void Init(const std::filesystem::path& exeDir)
    {
        EXECUTABLE_DIR = exeDir;
    }
}
