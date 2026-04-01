//
// Created by belle on 30/03/2026.
//

#pragma once
#include "Lumiere/Node3D.h"
#include "Lumiere/Components/Camera.h"
#include "Lumiere/Events/EventHandler.h"
#include "Lumiere/Renderer/Camera.h"

namespace lum
{
class CameraSystem : public ISystem
{
public:
    enum CameraMode
    {
        Editor,
        Play,
        EditorInPlay
    };
private:
    uint32_t m_viewportWidth {1};
    uint32_t m_viewportHeight {1};
    rdr::Camera* m_editorCamera {nullptr};
    Node3D* m_playerCamera {nullptr};

    CameraMode m_mode {CameraMode::Editor};
    rdr::CameraData m_cameraData {};

    std::shared_ptr<evt::EventHandler> m_events;

    void UploadGameCameraData();
    void UploadEditorCameraData();
public:
    CameraSystem(uint32_t width, uint32_t height, const std::shared_ptr<evt::EventHandler>& events);

    void Update(float dt) override;

    void SetPlayerCamera(Node3D* cam) { m_playerCamera = cam; }
    void SetEditorCamera(rdr::Camera* cam) { m_editorCamera = cam; }
    /**
     * \brief Enables or disables the window cursor.
     * TODO DOES IT MAKE SENSE TO HAVE THIS HERE? NOT THAT MUCH BUT IT'S CONVENIENT AND I CAN HIDE IT IN BINDINGS
     * \param visible true to show the cursor, false to hide it
     */
    void SetCursorVisible(bool visible) const;

    rdr::CameraData& CameraData() { return m_cameraData; }

    void SetEditorMode() {m_mode = Editor; }
    void SetPlayMode() {m_mode = Play; }
    void SetEditorInPlayMode() {m_mode = EditorInPlay; }
    CameraMode CurrentMode() const { return m_mode; }

    void OnEvent(const std::shared_ptr<evt::IEvent>& e);
};
} // lum