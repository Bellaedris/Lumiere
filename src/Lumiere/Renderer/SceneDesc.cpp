//
// Created by Bellaedris on 20/01/2026.
//

#include "SceneDesc.h"

#include <queue>
#include <stack>

#include "Lumiere/Components/MeshRenderer.h"
#include "Lumiere/Utils/MeshLoader.h"

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

void SceneDesc::AddMesh(const std::string &path)
{
    utils::MeshLoader loader;
    m_meshes.emplace_back(loader.LoadMeshFromFile(path));
}

std::vector<SceneDesc::RenderInstance> SceneDesc::RenderInstances()
{
    std::vector<SceneDesc::RenderInstance> renderInstances;

    std::stack<Node3D*> stack;
    stack.push(m_rootNode.get());
    // iterate through the scene to find MeshRenderers
    Node3D* current;
    while (stack.empty() == false)
    {
        current = stack.top();
        stack.pop();

        std::optional<comp::MeshRenderer*> comp = current->GetComponent<comp::MeshRenderer>();
        if (comp.has_value() && comp.value()->Mesh() != nullptr)
        {
            renderInstances.emplace_back(comp.value()->Mesh(), current->GetTransform().Model());
        }

        for (auto&& child : current->Children())
        {
            stack.push(child.get());
        }
    }

    return renderInstances;
}

void SceneDesc::ForEach(const std::function<void(const gfx::SubMesh &primitive, const gfx::MaterialPtr &material)> &callback) const
{
    for (const auto& mesh : m_meshes)
        for (const gfx::SubMesh& submesh : mesh->Primitives())
            callback(submesh, submesh.Material());
}
} // lum::rdr