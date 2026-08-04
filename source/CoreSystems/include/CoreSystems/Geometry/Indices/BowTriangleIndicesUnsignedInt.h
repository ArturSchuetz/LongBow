#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Geometry/Indices/IBowIndicesBase.h"

#include <CoreSystems/BowLogger.h>

namespace bow
{

struct CORESYSTEMS_API TriangleIndicesUnsignedInt
{
  public:
    TriangleIndicesUnsignedInt(uint32_t ui0, uint32_t ui1, uint32_t ui2) : m_index0(ui0), m_index1(ui1), m_index2(ui2) { FN("TriangleIndicesUnsignedInt::TriangleIndicesUnsignedInt"); }

    uint32_t GetIndex(uint32_t index)
    {
        FN("TriangleIndicesUnsignedInt::GetIndex");

        switch (index)
        {
        case 0:
            return m_index0;
            break;
        case 1:
            return m_index1;
            break;
        case 2:
            return m_index2;
            break;
        default:
            return -1;
            break;
        }
    }

  private:
    uint32_t m_index0;
    uint32_t m_index1;
    uint32_t m_index2;
};

} // namespace bow
