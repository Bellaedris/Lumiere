#include "Lumiere/App.h"

class AppTest : public lum::App
{
public:
    AppTest(int width, int height) : App(width, height, 4, 6) {};

    void Init() override
    {
        // initialize what needs to be rendered here
        // camera
        m_camera = std::make_unique<lum::Camera>(glm::vec3(0, 0, 0), m_window.AspectRatio(), 70.f, .01f, 1000.f);

        f = std::make_unique<lum::gpu::Framebuffer>(m_window.Width(), m_window.Height());
        f->Attach(lum::gpu::Framebuffer::Attachment::Color);
        f->Attach(lum::gpu::Framebuffer::Attachment::Depth);
        f->Unbind(lum::gpu::Framebuffer::Type::ReadWrite);

        // simple shader
        s.AddShaderFromFile(lum::gpu::Shader::Vertex, "shaders/default.vert");
        s.AddShaderFromFile(lum::gpu::Shader::Fragment, "shaders/default.frag");
        s.Create();

        compute.AddShaderFromFile(lum::gpu::Shader::Compute, "shaders/tonemap.comp");
        compute.Create();

        // meshes to draw
        m_meshes.push_back(std::move(lum::Mesh("resources/models/backpack.obj")));
        m_meshes.push_back(std::move(lum::Mesh::GeneratePlane(1.f)));
        t = std::make_unique<lum::gpu::Texture>(lum::gpu::Texture::TextureTarget::Target2D, "resources/models/diffuse.jpg", true);
        m_tonemappingTexture = std::make_unique<lum::gpu::Texture>(lum::gpu::Texture::TextureTarget::Target2D);
        m_tonemappingTexture->SetSize(m_window.Width(), m_window.Height());
        m_tonemappingTexture->SetMinFilter(lum::gpu::Texture::Linear);
        m_tonemappingTexture->SetMagFilter(lum::gpu::Texture::Linear);
        m_tonemappingTexture->Allocate(lum::gpu::Texture::RGBA, lum::gpu::GLUtils::UnsignedByte);

        lum::gpu::GLUtils::ClearColor({.2f, .2f, .2f, 1.f});
        lum::gpu::GLUtils::SetDepthTesting(true);
    }

    void Render() override
    {
        // clear the framebuffer
        lum::gpu::GLUtils::Clear();

        // Render the frame
        f->Bind(lum::gpu::Framebuffer::Type::Write);
        lum::gpu::GLUtils::SetDepthTesting(true);
        lum::gpu::GLUtils::Clear();

        s.Bind();
        s.UniformData("viewMatrix", m_camera->View());
        s.UniformData("projectionMatrix", m_camera->Projection());
        t->Bind(0);
        s.UniformData("albedo", 0);
        for(const auto& mesh : m_meshes)
            mesh.Draw();

        // post processing
        compute.Bind();
        m_tonemappingTexture->BindImage(0, 0, lum::gpu::GLUtils::Write);
        f->ColorTexture()->Bind(1);
        compute.UniformData("framebuffer", 1);

        compute.Dispatch(1360, 768, 1);
        compute.Wait();

        f->Unbind(lum::gpu::Framebuffer::Type::ReadWrite);
        lum::gpu::GLUtils::SetViewportSize(1360, 768);

        lum::gpu::Framebuffer::Attachment whatToCopy = lum::gpu::Framebuffer::Attachment::Color;
        lum::gpu::Framebuffer::BlitFramebuffer(f, nullptr, 0, 0, 1360, 768, 0, 0, 1360, 768, whatToCopy, lum::gpu::Texture::Filtering::Linear);
        m_gpuDeltaTime.End();
    }

private:
    lum::gpu::Shader s;
    lum::gpu::Shader compute;
    std::unique_ptr<lum::gpu::Texture> t;
    std::unique_ptr<lum::gpu::Texture> m_tonemappingTexture;
    std::unique_ptr<lum::gpu::Framebuffer> f;
};

int main()
{
    AppTest app(1360, 768);
    app.Run();
}
