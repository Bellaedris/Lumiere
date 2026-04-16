//
// Created by belle on 23/02/2026.
//

#include "Node3D.h"
#include <stduuid/uuid.h>

#include <utility>

#include "Components/Transform.h"
#include "Components/Light.h"
#include "Components/MeshRenderer.h"
#include "Systems/CameraSystem.h"
#include "Systems/ScriptEngine.h"
#include "Systems/PhysicsSystem.h"

namespace lum
{
bool Node3D::m_registered = false;

void Node3D::RegisterType()
{
    m_registered = true;

    sol::state& lua = m_systemProvider->m_scripting->State();
    // since Transforms are always created alongside Nodes, we register the 2 types at the same time so we don't have to
    // give any scripting context to Transform post-creation, which would be very ugly.

    auto vec2Mul = sol::overload( []( const glm::vec2& v1, const glm::vec2& v2 ) { return v1 * v2; },
                                                  []( const glm::vec2& v1, float value ) { return v1 * value; },
                                                  []( float value, const glm::vec2& v1 ) { return v1 * value; } );

    // divider overloads
    auto vec2Div = sol::overload( []( const glm::vec2& v1, const glm::vec2& v2 ) { return v1 / v2; },
                                                []( const glm::vec2& v1, float value ) { return v1 / value; },
                                                []( float value, const glm::vec2& v1 ) { return v1 / value; } );

    // addition overloads
    auto vec2Add = sol::overload( []( const glm::vec2& v1, const glm::vec2& v2 ) { return v1 + v2; },
                                                  []( const glm::vec2& v1, float value ) { return v1 + value; },
                                                  []( float value, const glm::vec2& v1 ) { return v1 + value; } );

    // subtraction overloads
    auto vec2Sub = sol::overload( []( const glm::vec2& v1, const glm::vec2& v2 ) { return v1 - v2; },
                                                     []( const glm::vec2& v1, float value ) { return v1 - value; },
                                                     []( float value, const glm::vec2& v1 ) { return v1 - value; } );

    sol::usertype<glm::vec2> vec2 = lua.new_usertype<glm::vec2>("vec2",
            sol::call_constructor,
            sol::constructors<glm::vec2(float), glm::vec2(float, float)>(),
            sol::meta_function::multiplication,
            vec2Mul,
            sol::meta_function::division,
            vec2Div,
            sol::meta_function::addition,
            vec2Add,
            sol::meta_function::subtraction,
            vec2Sub,
            sol::meta_function::unary_minus,
            sol::overload([](const glm::vec2& v) { return - v;})
        );
    vec2["x"] = sol::property(
        [](const glm::vec2& v) { return v.x; },
        [](glm::vec2& v, float x) { v.x = x; }
    );
    vec2["y"] = sol::property(
        [](const glm::vec2& v) { return v.y; },
        [](glm::vec2& v, float y) { v.y = y; }
    );

    auto vec3Mul = sol::overload( []( const glm::vec3& v1, const glm::vec3& v2 ) { return v1 * v2; },
                                                  []( const glm::vec3& v1, float value ) { return v1 * value; },
                                                  []( float value, const glm::vec3& v1 ) { return v1 * value; } );

    // divider overloads
    auto vec3Div = sol::overload( []( const glm::vec3& v1, const glm::vec3& v2 ) { return v1 / v2; },
                                                []( const glm::vec3& v1, float value ) { return v1 / value; },
                                                []( float value, const glm::vec3& v1 ) { return v1 / value; } );

    // addition overloads
    auto vec3Add = sol::overload( []( const glm::vec3& v1, const glm::vec3& v2 ) { return v1 + v2; },
                                                  []( const glm::vec3& v1, float value ) { return v1 + value; },
                                                  []( float value, const glm::vec3& v1 ) { return v1 + value; } );

    // subtraction overloads
    auto vec3Sub = sol::overload( []( const glm::vec3& v1, const glm::vec3& v2 ) { return v1 - v2; },
                                                     []( const glm::vec3& v1, float value ) { return v1 - value; },
                                                     []( float value, const glm::vec3& v1 ) { return v1 - value; } );

    sol::usertype<glm::vec3> vec = lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor,
            sol::constructors<glm::vec3(glm::vec3), glm::vec3(float), glm::vec3(float, float, float)>(),
            sol::meta_function::multiplication,
            vec3Mul,
            sol::meta_function::division,
            vec3Div,
            sol::meta_function::addition,
            vec3Add,
            sol::meta_function::subtraction,
            vec3Sub,
            sol::meta_function::unary_minus,
            sol::overload([](const glm::vec3& v) { return - v;})
        );
    vec["x"] = sol::property(
        [](const glm::vec3& v) { return v.x; },
        [](glm::vec3& v, float x) { v.x = x; }
    );
    vec["y"] = sol::property(
        [](const glm::vec3& v) { return v.y; },
        [](glm::vec3& v, float y) { v.y = y; }
    );
    vec["z"] = sol::property(
        [](const glm::vec3& v) { return v.z; },
        [](glm::vec3& v, float z) { v.z = z; }
    );

    sol::usertype<glm::quat> quat = lua.new_usertype<glm::quat>("quat",
            sol::call_constructor,
            sol::constructors<glm::quat(float, float, float, float), glm::quat(glm::vec3)>()
        );
    quat["x"] = sol::property(
        [](const glm::quat& q) { return q.x; },
        [](glm::quat& q, float x) { q.x = x; }
    );
    quat["y"] = sol::property(
        [](const glm::quat& v) { return v.y; },
        [](glm::quat& v, float y) { v.y = y; }
    );
    quat["z"] = sol::property(
        [](const glm::quat& v) { return v.z; },
        [](glm::quat& v, float z) { v.z = z; }
    );
    quat["w"] = sol::property(
        [](const glm::quat& v) { return v.w; },
        [](glm::quat& v, float w) { v.w = w; }
    );

    // add rotation functions

    sol::usertype<comp::Transform> transform = lua.new_usertype<comp::Transform>("Transform");

    transform["Translate"] = &comp::Transform::Translate;
    transform["Rotate"] = &comp::Transform::Rotate;

    transform["localPosition"] = sol::property(
        [](comp::Transform& t) -> glm::vec3& { return t.m_position; },
        [](comp::Transform& t, const glm::vec3& p) { t.SetLocalPosition(p); }
    );

    transform["localRotation"] = sol::property(
        [](comp::Transform& t) -> glm::quat& { return t.m_rotation; },
        [](comp::Transform& t, const glm::quat& p) { t.m_rotation = p; }
    );

    transform["localScale"] = sol::property(
        [](comp::Transform& t) -> glm::vec3& { return t.m_scale; },
        [](comp::Transform& t, const glm::vec3& p) { t.m_scale = p; }
    );

    transform["position"] = sol::property(
        [](comp::Transform& t) -> glm::vec3 { return t.Position(); },
        [](comp::Transform& t, const glm::vec3& p) { t.SetPosition(p); }
    );
    transform["scale"] = sol::property(
        [](comp::Transform& t) -> glm::vec3 { return t.Scale(); },
        [](comp::Transform& t, const glm::vec3& p) { t.SetScale(p); }
    );
    transform["euler"] = sol::property(
        [](const comp::Transform& t) -> glm::vec3 {return t.EulerAngles();},
        [](comp::Transform& t, const glm::vec3& ea) { t.SetEulerAngles(ea); }
    );
    transform["forward"] = sol::property([](const comp::Transform& t) -> glm::vec3 {return t.Forward();});
    transform["right"] = sol::property([](const comp::Transform& t) -> glm::vec3 {return t.Right();});
    transform["up"] = sol::property([](const comp::Transform& t) -> glm::vec3 {return t.Up();});

    sol::usertype<Node3D> type = lua.new_usertype<Node3D>("Node3D");
    // there is no templated functions in lua so we have to bind all the possible get/add components functions by hand :(
    // LAMBDAS MUST RETURN REFERENCES
    type["transform"] = sol::property([](Node3D& node) -> comp::Transform* { return node.m_transform.get(); });
    type["GetMeshComponent"] = &Node3D::GetComponent<comp::MeshRenderer>;
    type["GetLightComponent"] = &Node3D::GetComponent<comp::Light>;
    type["GetRigidbodyComponent"] = &Node3D::GetComponent<comp::Rigidbody>;

    type["name"] = sol::property([](const Node3D& n) -> std::string {return n.GetName();});

    // Node3D should be the first registration to be called and therefore has to load all accessible systems
    // physics:
    sol::usertype<Ray> ray = lua.new_usertype<Ray>("Ray",
        sol::call_constructor,
        sol::constructors<Ray(const glm::vec3&, const glm::vec3&, float)>()
    );
    ray["origin"] = sol::property(
        [](Ray& r) -> glm::vec3 {return r.origin; },
        [](Ray& r, const glm::vec3& v) {r.origin = v; }
    );
    ray["direction"] = sol::property(
        [](Ray& r) -> glm::vec3 {return r.direction; },
        [](Ray& r, const glm::vec3& v) {r.direction = v; }
    );
    ray["maxHitDistance"] = sol::property(
        [](Ray& r) -> float {return r.maxHitDistance; },
        [](Ray& r, float d) {r.maxHitDistance = d; }
    );

    sol::usertype<RaycastResult> raycastResult = lua.new_usertype<RaycastResult>("RaycastResult");
    raycastResult["distance"] = sol::property([](RaycastResult& r) -> float { return r.distance; });
    raycastResult["node"] = sol::property([](RaycastResult& r) -> Node3D* { return r.node; });
    raycastResult["position"] = sol::property([](RaycastResult& r) -> glm::vec3 { return r.position; });
    raycastResult["normal"] = sol::property([](RaycastResult& r) -> glm::vec3 { return r.normal; });

    // register the type, but do not give it a constructor so scripters can't accidentally create a new Pḧysics system
    sol::usertype<PhysicsSystem> physics = lua.new_usertype<PhysicsSystem>("PhysicsSystem");

    physics["Raycast"] = &PhysicsSystem::Raycast;

    lua["Physics"] = m_systemProvider->m_physics;
}

Node3D::Node3D()
    : m_transform(std::make_unique<comp::Transform>(this, nullptr))
    , m_uuid(uuids::uuid_system_generator{}())
{

}

Node3D::Node3D(const std::string& name, uuids::uuid& uuid)
    : m_transform(std::make_unique<comp::Transform>(this, nullptr))
    , m_name(name)
    , m_uuid(std::move(uuid))
{

}

Node3D *Node3D::AddChild()
{
    auto child = std::make_unique<Node3D>();
    child->m_parent = this;
    child->m_depth = m_depth + 1;
    m_children.push_back(std::move(child));
    return m_children.back().get();
}

void Node3D::AddChild(std::unique_ptr<Node3D>& child)
{
    child->m_parent = this;
    child->m_depth = m_depth + 1;
    m_children.push_back(std::move(child));
}

void Node3D::RemoveChild(Node3D* child)
{
    auto iter = std::find_if(m_children.begin(), m_children.end(), [&child](const std::unique_ptr<Node3D>& node)
    {
        return node.get() == child;
    });
    if (iter != m_children.end())
    {
        m_children.erase(iter);
    }
}

void Node3D::TransferChild(Node3D *child, Node3D* destination)
{
    auto iter = std::find_if(m_children.begin(), m_children.end(), [&child](const std::unique_ptr<Node3D>& node)
    {
        return node.get() == child;
    });
    if (iter != m_children.end())
    {
        destination->AddChild(*iter);
        m_children.erase(iter);
    }
}

void Node3D::Update(float dt)
{
    if (m_transform->IsDirty())
    {
        UpdateSelfAndChildren(dt);
        return;
    }

    for (auto&& child : m_children)
        child->Update(dt);
}

void Node3D::UpdateSelfAndChildren(float dt)
{
    // if this node is dirty, update this node and its children. Otherwise, just go deeper and update the dirty nodes
    if (m_parent == nullptr)
        m_transform->UpdateModelMatrix();
    else
        m_transform->UpdateModelMatrix(m_parent->GetTransform()->Model());

    for (auto&& child : m_children)
        child->UpdateSelfAndChildren(dt);
}

void Node3D::SetSystemsContext(SystemProvider *systemProvider)
{
    m_systemProvider = systemProvider;
    if (m_registered == false)
        RegisterType();
}

void Node3D::AddComponent(std::unique_ptr<comp::IComponent> &component)
{
    m_components.push_back(std::move(component));
}

void Node3D::RemoveComponent(comp::IComponent *component)
{
    auto iter = std::find_if(m_components.begin(), m_components.end(), [&component](const std::unique_ptr<comp::IComponent>& comp)
    {
        return comp.get() == component;
    });
    if (iter != m_components.end())
    {
        m_components.erase(iter);
    }
}

bool Node3D::HasAncestor(Node3D *node) const
{
    const Node3D* current = this;
    while (current->m_parent != nullptr)
    {
        if (current->m_parent == node)
            return true;
        current = current->m_parent;
    }

    return false;
}
} // lum