#pragma once
#include <DirectX11RenderDevice/BowD3D11RenderDevicePredeclares.h>
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Context/VertexAttributeBindings/IBowVertexAttributeBindings.h>

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

namespace bow
{

typedef std::shared_ptr<class D3D11ShaderProgram> D3D11ShaderProgramPtr;

//! Which buffers feed which shader inputs, plus the input layout for it.
/*!
    OpenGL records this in a vertex array object; DirectX 11 splits it into an
    ID3D11InputLayout describing the format and a separate call binding the
    buffers. The layout can only be created against a compiled vertex shader,
    because DirectX validates it against the signature the shader declares, so
    it is built lazily the first time a draw names a program.

    Each attribute has a vertex buffer of its own here, which becomes one input
    slot each -- the same arrangement glVertexArrayVertexBuffer produces on the
    OpenGL side.
*/
class D3D11VertexAttributeBindings : public IVertexAttributeBindings
{
  public:
    D3D11VertexAttributeBindings(ID3D11Device *device, ID3D11DeviceContext *context);
    ~D3D11VertexAttributeBindings();

    VertexBufferAttributeMap VGetAttributes() override;
    void VSetAttribute(uint32_t location, VertexBufferAttributePtr pointer) override;
    void VSetAttribute(ShaderVertexAttributePtr vertexAttribute, VertexBufferAttributePtr pointer) override;

    IndexBufferPtr VGetIndexBuffer() override;
    void VSetIndexBuffer(IndexBufferPtr pointer) override;

    //! Binds the layout, the vertex buffers and the index buffer for a draw.
    /*!
        \param program The program the layout has to match.
        \return false when no layout could be built, in which case nothing was bound.
    */
    bool Bind(const D3D11ShaderProgramPtr &program);

    //! Highest attribute index plus one, i.e. how many vertices a full draw covers.
    uint32_t GetVertexCount() const;

  private:
    D3D11VertexAttributeBindings(const D3D11VertexAttributeBindings &) = delete;
    D3D11VertexAttributeBindings &operator=(const D3D11VertexAttributeBindings &) = delete;

    bool EnsureInputLayout(const D3D11ShaderProgramPtr &program);

    ID3D11Device *m_device;
    ID3D11DeviceContext *m_context;

    VertexBufferAttributeMap m_attributes;
    IndexBufferPtr m_indexBuffer;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    //! The program the current layout was built for; a different one rebuilds it.
    const void *m_layoutProgram;
    bool m_dirty;
};

typedef std::shared_ptr<D3D11VertexAttributeBindings> D3D11VertexAttributeBindingsPtr;

} // namespace bow
