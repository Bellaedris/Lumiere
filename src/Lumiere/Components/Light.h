//
// Created by belle on 06/03/2026.
//

#pragma once
#include "IComponent.h"
#include "glm/vec3.hpp"

namespace lum::comp
{
class Light : public IComponent
{
private:
    glm::vec3 m_color {1.f, 1.f, 1.f};
    float m_intensity {1.f};

    float m_pointRange {1.f};

    int m_selectedType {0};

    /** \brief registration to the component factory status */
    static bool m_registered;
    /** \brief lua type registration status */
    static bool m_typeRegistered;
    void RegisterType();
public:
    #pragma region Constants
    static constexpr int LIGHT_TYPE_COUNT = 3;
    static constexpr const char* LIGHT_TYPES[] = {"Directional", "Point", "Spot"};
    #pragma endregion // Constants
    Light(Node3D* node);

    /**
     * \brief Type of this light component
     * \return 0 for directional, 1 for point, 2 for spot
     */
    int&      Type() { return m_selectedType; };
    glm::vec3&Color() { return m_color; };
    float&    Intensity() { return m_intensity; };
    float&    PointRange() { return m_pointRange; }

    void      Serialize(YAML::Node node) override;
    void      Deserialize(YAML::Node node) override;;
};
} // lum::comp