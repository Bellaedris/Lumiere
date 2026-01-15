//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#include <string>
#include <optional>

#include "assimp/scene.h"
#include "Lumiere/Actor.h"

namespace lum::utils
{
    class FileUtils
    {
    public:
        static std::optional<std::string> read_file(const char* filename);
    };
} // mgl::utils