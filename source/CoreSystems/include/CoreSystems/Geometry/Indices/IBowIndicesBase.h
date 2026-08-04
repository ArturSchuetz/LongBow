#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include <CoreSystems/BowLogger.h>

namespace bow
{

enum class IndicesType : char
{
    UnsignedShort,
    UnsignedInt
};

class CORESYSTEMS_API IIndicesBase
{
  protected:
    IIndicesBase(IndicesType type) : Type(type) { FN("IIndicesBase::IIndicesBase"); }

  public:
    virtual ~IIndicesBase() { FN("IIndicesBase::~IIndicesBase"); }

    virtual uint32_t Size() = 0;

  public:
    const IndicesType Type;
};

typedef std::shared_ptr<IIndicesBase> IndicesBasePtr;

} // namespace bow
