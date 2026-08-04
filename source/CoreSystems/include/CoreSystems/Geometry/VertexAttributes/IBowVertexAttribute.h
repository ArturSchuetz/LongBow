#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include <CoreSystems/BowLogger.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace bow
{

enum class VertexAttributeType : char
{
    UnsignedByte,
    Float,
    FloatVector2,
    FloatVector3,
    FloatVector4,
};

struct IVertexAttribute
{
  protected:
    IVertexAttribute(const std::string &name, VertexAttributeType type) : Name(name), Type(type) { FN("IVertexAttribute::IVertexAttribute"); }
    virtual ~IVertexAttribute() { FN("IVertexAttribute::~IVertexAttribute"); }

  public:
    const std::string Name;
    const VertexAttributeType Type;
};

template <class T> struct VertexAttribute : IVertexAttribute
{
  protected:
    VertexAttribute(const std::string &name, VertexAttributeType type) : IVertexAttribute(name, type), Values(std::vector<T>()) { FN("VertexAttribute::VertexAttribute"); }

    VertexAttribute(const std::string &name, VertexAttributeType type, int capacity) : IVertexAttribute(name, type), Values(std::vector<T>(capacity)) { FN("VertexAttribute::VertexAttribute"); }

  public:
    std::vector<T> Values;
};

typedef std::shared_ptr<IVertexAttribute> VertexAttributePtr;
typedef std::unordered_map<std::string, VertexAttributePtr> VertexAttributeMap;

} // namespace bow
