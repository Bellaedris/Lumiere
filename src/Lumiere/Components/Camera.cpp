//
// Created by belle on 30/03/2026.
//

#include "Camera.h"

#include "sol.hpp"
#include "Lumiere/Systems/ScriptEngine.h"
#include "Lumiere/Utils/YAMLUtils.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(Camera, "Camera")
bool Camera::m_typeRegistered = false;
Camera::Camera(Node3D *node, SystemProvider *systems)
    : IComponent(node, systems)
{
    if (m_typeRegistered == false)
    {
        m_typeRegistered = true;
        sol::state& lua = systems->m_scripting->State();

        sol::usertype<Camera> cam = lua.new_usertype<Camera>("Camera");
        cam["fov"] = sol::property(
            [](const Camera& c) { return c.m_fov; },
            [](Camera& c, float fov) { c.m_fov = fov; }
        );

        cam["znear"] = sol::property(
            [](const Camera& c) { return c.m_zNear; },
            [](Camera& c, float znear) { c.m_zNear = znear; }
        );

        cam["zfar"] = sol::property(
            [](const Camera& c) { return c.m_zFar; },
            [](Camera& c, float zfar) { c.m_zFar = zfar; }
        );
    }
}

std::string Camera::CameraType() const
{
    return m_perspective ? "Perspective" : "Orthographic";
}

void Camera::Serialize(YAML::Node& node)
{
    YAML::Node cam;
    cam["componentType"] = "Camera";
    cam["perspective"] = m_perspective;
    cam["znear"] = m_zNear;
    cam["zfar"] = m_zFar;
    cam["fov"] = m_fov;
    cam["orthographicSize"] = m_orthographicSize;
    cam["viewportMin"] = m_viewportMin;
    cam["viewportMax"] = m_viewportMax;
    cam["viewportWidth"] = m_viewportWidth;
    cam["viewportHeight"] = m_viewportHeight;
    node.push_back(cam);
}

void Camera::Deserialize(YAML::Node& node)
{
    m_perspective = node["perspective"].as<bool>();
    m_zNear = node["znear"].as<float>();
    m_zFar = node["zfar"].as<float>();
    m_fov = node["fov"].as<float>();
    m_orthographicSize = node["orthographicSize"].as<float>();
    m_viewportMin = node["viewportMin"].as<glm::vec2>();
    m_viewportMax = node["viewportMax"].as<glm::vec2>();
    m_viewportWidth = node["viewportWidth"].as<float>();
    m_viewportHeight = node["viewportHeight"].as<float>();
}
} // lum