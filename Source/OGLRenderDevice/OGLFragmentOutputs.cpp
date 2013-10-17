#pragma once

#include "OGLFragmentOutputs.h"
#include "OGLShaderProgramName.h"

#include <GL/glew.h>

namespace Bow {


OGLFragmentOutputs::OGLFragmentOutputs(OGLShaderProgramNamePtr program)
{
	FN("OGLFragmentOutputs::OGLFragmentOutputs()");
	LOG_ASSERT(program.get() != nullptr, "OGLShaderProgramNamePtr is a nullptr.");

    m_program = program;
}


OGLFragmentOutputs::~OGLFragmentOutputs()
{
	FN("OGLFragmentOutputs::~OGLFragmentOutputs()");
}


int OGLFragmentOutputs::operator[](std::string index) const
{
	FN("OGLFragmentOutputs::operator[]()");
	unsigned int i = -1;
	i = glGetFragDataLocation(m_program->GetValue(), index.c_str());

    if (i == -1)
    {
        LOG_ASSERT(i != -1, "Fragment Output Key does not exist.");
		return -1;
    }

    return i;
}


}