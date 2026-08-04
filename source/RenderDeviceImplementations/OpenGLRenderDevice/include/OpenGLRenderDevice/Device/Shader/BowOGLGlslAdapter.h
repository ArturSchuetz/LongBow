#pragma once
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>

#include <string>
#include <unordered_map>

namespace bow
{

//! Makes Vulkan-flavoured GLSL compile as desktop GLSL.
/*!
    The examples are written once and run on every backend, which means their
    shaders are authored for Vulkan. Two constructs in that dialect are not
    accepted by a desktop GLSL compiler:

      layout(set = N, binding = M)   descriptor sets do not exist in OpenGL
      layout(push_constant)          neither do push constants

    Rewriting them is enough because this backend resolves every resource by
    name -- glGetUniformBlockIndex, glGetProgramResourceIndex,
    glGetUniformLocation -- and then assigns the binding point itself. The
    numbers written in the shader are therefore never relied upon, so dropping
    the set qualifier cannot introduce a conflict.
*/
class GlslAdapter
{
  public:
    struct Result
    {
        std::string source;

        //! Instance name to block name for each converted push-constant block.
        /*!
            GLSL exposes the block name to the API, not the instance name, but
            callers address a push constant by the instance name they wrote in
            the shader. The mapping is recorded while rewriting because it
            cannot be recovered from the linked program afterwards.
        */
        std::unordered_map<std::string, std::string> pushConstantBlocks;
    };

    //! Rewrites Vulkan GLSL into desktop GLSL. Other sources pass through.
    static Result Adapt(const std::string &source);
};

} // namespace bow
