#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include <iomanip>
#include <random>
#include <sstream>

namespace bow
{

class CORESYSTEMS_API Utils
{
  public:
    static std::string GenerateGUID();
};

} // namespace bow
