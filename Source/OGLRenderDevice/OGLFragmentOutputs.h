#pragma once
#include "LongBow.h"

#include "IBowFragmentOutputs.h"
#include "OGLShaderProgramName.h"

namespace Bow {


class OGLFragmentOutputs : public IFragmentOutputs
{
public:
	OGLFragmentOutputs(OGLShaderProgramNamePtr program);
	~OGLFragmentOutputs();

    int operator[](std::string index) const;

private:
	OGLShaderProgramNamePtr m_program;
};

typedef std::shared_ptr<OGLFragmentOutputs> OGLFragmentOutputsPtr;

}