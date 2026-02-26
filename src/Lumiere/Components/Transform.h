//
// Created by belle on 23/02/2026.
//

#pragma once
#include "IComponent.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace lum::comp
{
class Transform : public IComponent
{
private:
    /** \brief World space datas */
    glm::mat4 m_model         {glm::mat4(1.f)};

    glm::vec3 m_position      {glm::vec3(.0f)};
    glm::vec3 m_rotationEuler {glm::vec3(.0f)};
    glm::vec3 m_scale         {glm::vec3(1.f)};

    bool m_isDirty {false};

    glm::mat4 LocalModelMatrix() const;
public:
    void UpdateModelMatrix();
    void UpdateModelMatrix(const glm::mat4& parent);

    void SetLocalPosition(const glm::vec3& newPosition);
    void SetLocalRotation(const glm::vec3& newRotation);
    void SetLocalScale(const glm::vec3& newScale);

    [[nodiscard]] const glm::mat4&Model() const {return m_model;}
    [[nodiscard]] glm::vec3       Position() const { return m_model[3]; }
    [[nodiscard]] glm::vec3       Right() const { return m_model[0]; };
    [[nodiscard]] glm::vec3       Up() const { return m_model[1]; }
    [[nodiscard]] glm::vec3       Forward() const { return -m_model[2]; };
    [[nodiscard]] glm::vec3       Scale() const { return {glm::length(Right()), glm::length(Up()), glm::length(Forward())}; }
    [[nodiscard]] bool            IsDirty() const { return m_isDirty; }

    void DrawInspector() override;
};
} // lum