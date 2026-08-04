#include "CoreSystems/BowUtils.h"

#include <CoreSystems/BowLogger.h>

namespace bow
{

std::string Utils::GenerateGUID()
{
    FN("Utils::GenerateGUID");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 8; ++i)
    {
        ss << std::hex << std::uppercase << dis(gen);
        if ((i + 1) % 2 == 0 && i < 7) // Füge nach jedem zweiten Zeichen einen Bindestrich hinzu, außer am Ende
        {
            ss << "-";
        }
    }

    return ss.str();
}

} // namespace bow
