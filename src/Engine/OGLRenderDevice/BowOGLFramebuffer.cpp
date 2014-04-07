#include "BowOGLFramebuffer.h"
#include "BowLogger.h"

#include "BowOGLFramebufferName.h"
#include "BowOGLTextureName.h"
#include "BowOGLTexture2D.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		OGLFramebuffer::OGLFramebuffer() : m_name(new OGLFramebufferName())
		{
		}

		OGLFramebuffer::~OGLFramebuffer()
		{
		}

		void OGLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_name->GetValue());
		}

		void OGLFramebuffer::UnBind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void OGLFramebuffer::Clean()
		{
			if (m_colorAttachments.IsDirty)
			{
				OGLColorAttachmentMap colorAttachments = m_colorAttachments.Attachments;

				GLenum *drawBuffers = new GLenum[colorAttachments.size()];
				for (auto it = colorAttachments.begin(); it != colorAttachments.end(); it++)
				{
					if ((*it).second.Dirty)
					{
						Attach(GL_COLOR_ATTACHMENT0 + (*it).first, (*it).second.Texture);
						(*it).second.Dirty = false;
					}

					if ((*it).second.Texture != nullptr)
					{
						drawBuffers[(*it).first] = GL_COLOR_ATTACHMENT0 + (*it).first;
					}
				}
				glDrawBuffers(colorAttachments.size(), drawBuffers);
				delete[] drawBuffers;

				m_colorAttachments.IsDirty = false;
			}

			if (m_dirtyFlags != DirtyFlags::None)
			{
				if ((DirtyFlags)((int)m_dirtyFlags & (int)DirtyFlags::DepthAttachment) == DirtyFlags::DepthAttachment)
				{
					Attach(GL_DEPTH_ATTACHMENT, m_depthAttachment);
				}

				//
				// The depth-stencil attachment overrides the depth attachment:
				//
				//    "Attaching a level of a texture to GL_DEPTH_STENCIL_ATTACHMENT 
				//     is equivalent to attaching that level to both the 
				//     GL_DEPTH_ATTACHMENT and the GL_STENCIL_ATTACHMENT attachment 
				//     points simultaneously."
				//
				// We do not expose just a stencil attachment because TextureFormat
				// does not contain a stencil only format.
				//
				if ((DirtyFlags)((int)m_dirtyFlags & (int)DirtyFlags::DepthStencilAttachment) == DirtyFlags::DepthStencilAttachment)
				{
					Attach(GL_DEPTH_STENCIL_ATTACHMENT, m_depthStencilAttachment);
				}

				m_dirtyFlags = DirtyFlags::None;
			}
		}

		Texture2DPtr OGLFramebuffer::GetColorAttachment(int index) const
		{
			return m_colorAttachments.GetAttachment(index);
		}

		void OGLFramebuffer::SetColorAttachment(int index, Texture2DPtr texture)
		{
			m_colorAttachments.SetAttachment(index, texture);
		}

		Texture2DPtr OGLFramebuffer::GetDepthAttachment(void) const
		{
			return m_depthAttachment;
		}

		void OGLFramebuffer::SetDepthAttachment(Texture2DPtr texture)
		{
			if (m_depthAttachment != texture)
			{
				LOG_ASSERT(!((texture != nullptr) && (!texture->GetDescription().DepthRenderable())), "Texture must be depth renderable but the Description.DepthRenderable property is false.");

				m_depthAttachment = std::dynamic_pointer_cast<OGLTexture2D>(texture);
				m_dirtyFlags = (DirtyFlags)((int)m_dirtyFlags | (int)DirtyFlags::DepthAttachment);
			}
		}


		Texture2DPtr OGLFramebuffer::GetDepthStencilAttachment(void) const
		{
			return m_depthStencilAttachment;
		}

		void OGLFramebuffer::SetDepthStencilAttachment(Texture2DPtr texture)
		{
			if (m_depthStencilAttachment != texture)
			{
				LOG_ASSERT(!((texture != nullptr) && (!texture->GetDescription().DepthStencilRenderable())), "Texture must be depth/stencil renderable but the Description.DepthStencilRenderable property is false.");

				m_depthStencilAttachment = std::dynamic_pointer_cast<OGLTexture2D>(texture);
				m_dirtyFlags = (DirtyFlags)((int)m_dirtyFlags | (int)DirtyFlags::DepthStencilAttachment);
			}
		}

		void OGLFramebuffer::Attach(unsigned int attachPoint, OGLTexture2DPtr texture)
		{
			if (texture != nullptr)
			{
				// TODO:  Mipmap level
				glFramebufferTexture(GL_FRAMEBUFFER, attachPoint, texture->GetHandle()->GetValue(), 0);
			}
			else
			{
				glFramebufferTexture(GL_FRAMEBUFFER, attachPoint, 0, 0);
			}
		}

	}
}