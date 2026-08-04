#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Geometry/Indices/IBowIndicesBase.h"
#include "CoreSystems/Geometry/VertexAttributes/IBowVertexAttribute.h"

#include <CoreSystems/BowLogger.h>

namespace bow
{

// ---------------------------------------------------------------------------
/** @brief A mesh represents geometry without any material.
 */
class MeshAttribute
{
  public:
    VertexAttributePtr GetAttribute(std::string name)
    {
        FN("MeshAttribute::GetAttribute");

        return m_attributes.find(name)->second;
    };

    void AddAttribute(VertexAttributePtr vertexAttribute)
    {
        FN("MeshAttribute::AddAttribute");

        m_attributes.insert(std::pair<std::string, VertexAttributePtr>(vertexAttribute->Name, vertexAttribute));
    }

    IndicesBasePtr Indices;

    MeshAttribute() : m_attributes(), Indices(nullptr) { FN("MeshAttribute::MeshAttribute"); }
    ~MeshAttribute() { FN("MeshAttribute::~MeshAttribute"); }

  private:
    VertexAttributeMap m_attributes;
};
} // namespace bow
