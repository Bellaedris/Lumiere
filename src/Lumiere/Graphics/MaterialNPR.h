//
// Created by Bellaedris on 14/01/2026.
//

#pragma once

#include "IMaterial.h"

namespace lum::gfx
{
class MaterialNPR : public IMaterial
{
public:
    void Bind(const gpu::ShaderPtr &shader) override;
    void DrawEditor() override;

};
} // lum