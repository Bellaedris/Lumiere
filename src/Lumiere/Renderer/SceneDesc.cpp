//
// Created by Bellaedris on 20/01/2026.
//

#include "SceneDesc.h"

#include <fstream>
#include <stack>

#include "Lumiere/Components/Light.h"
#include "Lumiere/Components/MeshRenderer.h"
#include "Lumiere/Utils/MeshLoader.h"
#include "yaml-cpp/node/convert.h"
#include <yaml-cpp/yaml.h>

namespace lum::rdr
{
SceneDesc::SceneDesc()
    : m_rootNode(std::make_unique<Node3D>())
{
}

Node3D *SceneDesc::AddNode()
{
    return m_rootNode->AddChild();
}

std::vector<SceneDesc::RenderInstance> SceneDesc::RenderInstances()
{
    std::vector<SceneDesc::RenderInstance> renderInstances;

    ForEachNode([&](Node3D* current)
    {
        std::optional<comp::MeshRenderer*> comp = current->GetComponent<comp::MeshRenderer>();
        if (comp.has_value() && comp.value()->Mesh() != nullptr)
        {
            renderInstances.emplace_back(comp.value()->Mesh(), current->GetTransform().Model());
        }
    });

    return renderInstances;
}

void SceneDesc::GatherAndUploadLights()
{
    m_lights->Clear();

    ForEachNode([&](Node3D* current)
    {
        std::optional<comp::Light*> l = current->GetComponent<comp::Light>();
        if (l.has_value())
        {
            comp::Light* light = l.value();
            switch (light->Type())
            {
                case 0:
                    m_lights->AddDirLight(current->GetTransform().Forward(), light->Intensity(), light->Color());
                    break;
                case 1:
                    m_lights->AddPointLight(current->GetTransform().Position(), light->Intensity(), light->Color(), light->PointRange());
                    break;
                    case 2:
                    // not implemented
                    break;
                default:
                    std::cerr << "Unknown light type\n";
            }
        }
    });
}

void SceneDesc::ForEachNode(const std::function<void(Node3D *node)> &callback)
{
    std::stack<Node3D*> stack;
    stack.push(m_rootNode.get());
    // iterate through the scene to find MeshRenderers
    Node3D* current;
    while (stack.empty() == false)
    {
        current = stack.top();
        stack.pop();

        // since the root node never has components, we won't be doing anything with it
        if (current != m_rootNode.get())
            callback(current);

        for (auto&& child : current->Children())
        {
            stack.push(child.get());
        }
    }
}

void SceneDesc::Serialize()
{
    YAML::Node root;
    root["version"] = SCENE_DESC_SERIALIZATION_VERSION;
    root["name"] = m_rootNode->Name();
    root["uuid"] = to_string(m_rootNode->UUID());

    // i don't want another member function for the recursion....
    std::function<void(YAML::Node, Node3D*)> serialize = [&](YAML::Node parent, Node3D* node) {
        YAML::Node n;
        n["name"] = node->Name();
        n["uuid"] = to_string(node->UUID());
        node->GetTransform().Serialize(n);
        YAML::Node components = n["components"];
        for (auto&& component : node->Components())
            component->Serialize(components);

        parent["children"].push_back(n);
        for (auto&& child : node->Children())
            serialize(n, child.get());
    };

    for (auto&& child : m_rootNode->Children())
        serialize(root, child.get());

    std::ofstream out("Assets/" + RootNode()->Name() + SCENE_FILE_FORMAT);
    out << root;
}

void SceneDesc::Deserialize(const std::string& path)
{
    YAML::Node root;
    try
    {
        root = YAML::LoadFile(path);
    }
    catch (YAML::BadFile& e)
    {
        std::cerr << "Incompatible scene file.\n";
        return;
    }

    if (root["version"].as<int>() != SCENE_DESC_SERIALIZATION_VERSION)
    {
        std::cerr << "Serializer version mismatch. This scene uses deprecated serialization standards.\n";
        return;
    }

    std::optional<uuids::uuid> rootUUID = uuids::uuid::from_string(root["uuid"].as<std::string>());
    if (rootUUID.has_value() == false)
    {
        std::cerr << "This scene has a corrupted UUID.\n";
        return;
    }

    m_rootNode = std::make_unique<Node3D>(root["name"].as<std::string>(), rootUUID.value());
    // fill the children
    std::function<void(const YAML::Node&, Node3D*)> constructChildren = [&](const YAML::Node& node, Node3D* parent)
    {
        std::string name = node["name"].as<std::string>();
        std::optional<uuids::uuid> nodeUUID = uuids::uuid::from_string(node["uuid"].as<std::string>());
        if (rootUUID.has_value() == false)
        {
            std::cerr << "The node" << name << " has a corrupted UUID.\n";
            return;
        }
        std::unique_ptr<Node3D> n = std::make_unique<Node3D>(name, nodeUUID.value());
        n->GetTransform().Deserialize(node["transform"]);
        for (const auto& component : node["components"])
        {
            std::unique_ptr<comp::IComponent> comp = comp::ComponentFactory::Create(
                    component["componentType"].as<std::string>(),
                    n.get()
            );
            comp->Deserialize(static_cast<YAML::Node>(component));
            n->AddComponent(comp);
        }
        parent->AddChild(n);

        for (const auto& child : node["children"])
            constructChildren(child, parent->Children().at(parent->Children().size() - 1).get());
    };

    for (const auto& child : root["children"])
        constructChildren(child, m_rootNode.get());
}
} // lum::rdr