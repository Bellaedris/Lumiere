//
// Created by Bellaedris on 20/01/2026.
//

#include "SceneDesc.h"

#include <fstream>
#include <stack>

#include "Lumiere/Components/Light.h"
#include "Lumiere/Components/MeshRenderer.h"
#include "Lumiere/Utils/MeshLoader.h"
#include "Lumiere/Components/Transform.h"
#include "yaml-cpp/node/convert.h"
#include <yaml-cpp/yaml.h>

#include "Lumiere/Components/Camera.h"
#include "Lumiere/Components/UIElement.h"
#include "Lumiere/Systems/CameraSystem.h"

namespace lum::rdr
{
SceneDesc::SceneDesc(SystemProvider* systemProvider)
    : m_systemProvider(systemProvider)
    , m_rootNode(std::make_unique<Node3D>())
{
}

Node3D *SceneDesc::AddNode()
{
    Node3D* n = m_rootNode->AddChild();
    n->SetSystemsContext(m_systemProvider);
    return n;
}

std::unordered_map<gfx::MaterialPtr, std::vector<SceneDesc::RenderInstance>> SceneDesc::RenderInstances()
{
    std::unordered_map<gfx::MaterialPtr, std::vector<SceneDesc::RenderInstance>> renderInstances;

    ForEachNode([&](Node3D* current)
    {
        std::optional<comp::MeshRenderer*> comp = current->GetComponent<comp::MeshRenderer>();
        if (comp.has_value() && comp.value()->Mesh() != nullptr)
        {
            std::vector<gfx::SubMesh>& primitives = comp.value()->Mesh()->Primitives();
            std::vector<gfx::MaterialPtr> materials = comp.value()->Materials();
            size_t size = primitives.size();
            for (int i = 0; i < size; i++)
            {
                if (renderInstances.contains(materials[i]) == false)
                    renderInstances.emplace(materials[i], std::vector<SceneDesc::RenderInstance>());
                renderInstances[materials[i]].emplace_back(&primitives[i], current->GetTransform()->Model());
            }
        }
    });

    return renderInstances;
}

std::unordered_map<gfx::MaterialPtr, std::vector<SceneDesc::RenderInstance>> SceneDesc::UIRenderInstances()
{
    std::unordered_map<gfx::MaterialPtr, std::vector<SceneDesc::RenderInstance>> renderInstances;

    ForEachNode([&](Node3D* current)
    {
        std::optional<comp::UIElement*> comp = current->GetComponent<comp::UIElement>();
        if (comp.has_value() && comp.value()->Mesh() != nullptr)
        {
            std::vector<gfx::SubMesh>& primitives = comp.value()->Mesh()->Primitives();
            std::vector<gfx::MaterialPtr> materials = comp.value()->Materials();
            size_t size = primitives.size();
            for (int i = 0; i < size; i++)
            {
                if (renderInstances.contains(materials[i]) == false)
                    renderInstances.emplace(materials[i], std::vector<SceneDesc::RenderInstance>());
                renderInstances[materials[i]].emplace_back(&primitives[i], current->GetTransform()->Model());
            }
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
                    m_lights->AddDirLight(current->GetTransform()->Forward(), light->Intensity(), light->Color());
                    break;
                case 1:
                    m_lights->AddPointLight(current->GetTransform()->Position(), light->Intensity(), light->Color(), light->PointRange());
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

void SceneDesc::ClearNodes()
{
    m_rootNode = std::make_unique<Node3D>();
}

void SceneDesc::ResetScene()
{
    Serialize();
    ClearNodes();
}

void SceneDesc::LoadScene(const std::string &path)
{
    Serialize();
    Deserialize(path);
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

void SceneDesc::OnPlay()
{
    m_isPlaying = true;
    if (auto cam = FindComponentOfType<comp::Camera>(); cam != nullptr)
        m_systemProvider->m_camera->SetPlayerCamera(cam->Node());
    m_systemProvider->m_camera->SetPlayMode();

    ForEachNode([&](Node3D* current)
    {
        for (auto&& comp : current->Components())
            comp->OnPlay();
    });
}

void SceneDesc::OnStop()
{
    m_isPlaying = false;
    m_systemProvider->m_camera->SetEditorMode();
    ForEachNode([&](Node3D* current)
    {
        for (auto&& comp : current->Components())
            comp->OnStop();
    });
}

std::optional<std::string> SceneDesc::Serialize()
{
    YAML::Node root;
    root["version"] = SCENE_DESC_SERIALIZATION_VERSION;
    root["name"] = m_rootNode->Name();
    root["uuid"] = to_string(m_rootNode->UUID());

    // i don't want another member function for the recursion....
    std::function<void(YAML::Node&, Node3D*)> serialize = [&](YAML::Node& parent, Node3D* node) {
        YAML::Node n;
        n["name"] = node->Name();
        n["uuid"] = to_string(node->UUID());

        node->GetTransform()->Serialize(n);
        YAML::Node components;
        for (auto&& component : node->Components())
            component->Serialize(components);
        n["components"] = components;

        for (auto&& child : node->Children())
            serialize(n, child.get());
        parent["children"].push_back(n);
    };

    for (auto&& child : m_rootNode->Children())
        serialize(root, child.get());

    std::string path = "Assets/" + RootNode()->Name() + SCENE_FILE_FORMAT;
    YAML::Emitter emitter;
    emitter << root;
    if (emitter.good() == false)
    {
        std::cerr << "Couldn't serialize the scene " << RootNode()->Name() << ": " << emitter.GetLastError() << "\n";
        return {};
    }

    // we could eventually ensure the write process is correct by writing to a tmp file then renaming, but i'm kinda lazy
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    out << emitter.c_str();

    return path;
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
        YAML::Node transform = node["transform"];
        n->GetTransform()->Deserialize(transform);
        n->SetSystemsContext(m_systemProvider);
        for (auto& component : node["components"])
        {
            YAML::Node compNode = static_cast<YAML::Node>(component);
            std::unique_ptr<comp::IComponent> comp = comp::ComponentFactory::Create(
                    component["componentType"].as<std::string>(),
                    n.get(),
                    m_systemProvider
            );
            comp->Deserialize(compNode);
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