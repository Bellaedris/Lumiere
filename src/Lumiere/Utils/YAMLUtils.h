//
// Created by belle on 10/03/2026.
//

#pragma once

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace YAML
{
template<>
struct convert<glm::vec3>
{
    static Node encode(const glm::vec3& v)
    {
        Node node;
        node.push_back(v.x);
        node.push_back(v.y);
        node.push_back(v.z);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs) {
        if(!node.IsSequence() || node.size() != 3) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template<>
struct convert<glm::quat>
{
    static Node encode(const glm::quat& q)
    {
        Node node;
        node.push_back(q.x);
        node.push_back(q.y);
        node.push_back(q.z);
        node.push_back(q.w);
        return node;
    }

    static bool decode(const Node& node, glm::quat& rhs) {
        if(!node.IsSequence() || node.size() != 4) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};
}