#include "Resources/FileLoader/MeshLoader/BowModelLoader_ply.h"
#include "Resources/BowResources.h"



#include <iostream>
#include <sstream>

#include <algorithm>
#include <cstring>
#include <functional>
#include <type_traits>

namespace bow
{

template <typename T, typename T2> inline T2 endian_swap(const T &v)
{
    FN("endian_swap");

    return v;
}

template <> inline uint16_t endian_swap<uint16_t, uint16_t>(const uint16_t &v)
{
    FN("endian_swap");

    return (v << 8) | (v >> 8);
}

template <> inline uint32_t endian_swap<uint32_t, uint32_t>(const uint32_t &v)
{
    FN("endian_swap");

    return (v << 24) | ((v << 8) & 0x00ff0000) | ((v >> 8) & 0x0000ff00) | (v >> 24);
}

template <> inline uint64_t endian_swap<uint64_t, uint64_t>(const uint64_t &v)
{
    FN("endian_swap");

    return (((v & 0x00000000000000ffLL) << 56) | ((v & 0x000000000000ff00LL) << 40) | ((v & 0x0000000000ff0000LL) << 24) | ((v & 0x00000000ff000000LL) << 8) | ((v & 0x000000ff00000000LL) >> 8) | ((v & 0x0000ff0000000000LL) >> 24) |
            ((v & 0x00ff000000000000LL) >> 40) | ((v & 0xff00000000000000LL) >> 56));
}

template <> inline int16_t endian_swap<int16_t, int16_t>(const int16_t &v)
{
    FN("endian_swap");

    uint16_t r = endian_swap<uint16_t, uint16_t>(*(uint16_t *)&v);
    return *(int16_t *)&r;
}

template <> inline int32_t endian_swap<int32_t, int32_t>(const int32_t &v)
{
    FN("endian_swap");

    uint32_t r = endian_swap<uint32_t, uint32_t>(*(uint32_t *)&v);
    return *(int32_t *)&r;
}

template <> inline int64_t endian_swap<int64_t, int64_t>(const int64_t &v)
{
    FN("endian_swap");

    uint64_t r = endian_swap<uint64_t, uint64_t>(*(uint64_t *)&v);
    return *(int64_t *)&r;
}

template <> inline float endian_swap<uint32_t, float>(const uint32_t &v)
{
    FN("endian_swap");

    union
    {
        float f;
        uint32_t i;
    };
    i = endian_swap<uint32_t, uint32_t>(v);
    return f;
}

template <> inline float endian_swap<uint64_t, float>(const uint64_t &v)
{
    FN("endian_swap");

    union
    {
        float d;
        uint64_t i;
    };
    i = endian_swap<uint64_t, uint64_t>(v);
    return d;
}

// Convert string to floating point (real) type.
template <typename T> T stor(const std::string &substr)
{
    FN("stor");

    auto p = substr.begin();
    auto end = substr.end();
    T real = 0.0;
    bool negative = false;
    if (p != end && *p == '-')
    {
        negative = true;
        ++p;
    }
    while (p != end && *p >= '0' && *p <= '9')
    {
        real = (real * static_cast<T>(10.0)) + (*p - '0');
        ++p;
    }
    if (p != end && *p == '.')
    {
        T frac = 0.0;
        int n = 0;
        ++p;
        while (p != end && *p >= '0' && *p <= '9')
        {
            frac = (frac * static_cast<T>(10.0)) + (*p - '0');
            ++p;
            ++n;
        }
        real += frac / std::pow(static_cast<T>(10.0), n);
    }
    if (p != end && (*p == 'e' || *p == 'E'))
    {
        ++p;
        T sign = 1.0;
        if (p != end && *p == '-')
        {
            sign = -1.0;
            ++p;
        }
        T exponent = 0.0;
        while (p != end && *p >= '0' && *p <= '9')
        {
            exponent = (exponent * static_cast<T>(10.0)) + (*p - '0');
            ++p;
        }
        real = real * std::pow(static_cast<T>(10.0), sign * exponent);
    }
    if (negative)
    {
        real = -real;
    }
    return real;
}

// Convert string to unsigned type.
template <typename T> T stou(const std::string &substr)
{
    FN("stou");

    static_assert(std::is_unsigned<T>::value, "Cannot use stou() with signed type.");
    auto p = substr.begin();
    auto end = substr.end();
    T integer = 0;
    assert(*p != '-');
    while (p != end && *p >= '0' && *p <= '9')
    {
        integer = (integer * 10) + (*p - '0');
        ++p;
    }
    return integer;
}

// Convert string to signed integer type.
template <typename T> T stoi(const std::string &substr)
{
    FN("stoi");

    auto p = substr.begin();
    auto end = substr.end();
    T integer = 0;
    bool negative = false;
    if (p != end && *p == '-')
    {
        negative = true;
        ++p;
    }
    while (p != end && *p >= '0' && *p <= '9')
    {
        integer = (integer * 10) + (*p - '0');
        ++p;
    }
    if (negative)
    {
        integer = -integer;
    }
    return integer;
}

inline uint32_t hash_fnv1a(const std::string &str)
{
    FN("hash_fnv1a");

    static const uint32_t fnv1aBase32 = 0x811C9DC5u;
    static const uint32_t fnv1aPrime32 = 0x01000193u;
    uint32_t result = fnv1aBase32;
    for (auto &c : str)
    {
        result ^= static_cast<uint32_t>(c);
        result *= fnv1aPrime32;
    }
    return result;
}

inline Type property_type_from_string(const std::string &t)
{
    FN("property_type_from_string");

    if (t == "int8" || t == "char")
        return Type::INT8;
    else if (t == "uint8" || t == "uchar")
        return Type::UINT8;
    else if (t == "int16" || t == "short")
        return Type::INT16;
    else if (t == "uint16" || t == "ushort")
        return Type::UINT16;
    else if (t == "int32" || t == "int")
        return Type::INT32;
    else if (t == "uint32" || t == "uint")
        return Type::UINT32;
    else if (t == "float32" || t == "float")
        return Type::FLOAT32;
    else if (t == "float64" || t == "float")
        return Type::FLOAT64;
    return Type::INVALID;
}

inline size_t property_size_from_type(const Type &t)
{
    FN("property_size_from_type");

    if (t == Type::INT8 || t == Type::UINT8)
        return 1;

    else if (t == Type::INT16 || t == Type::UINT16)
        return 2;

    else if (t == Type::INT32 || t == Type::UINT32 || t == Type::FLOAT32)
        return 4;

    else if (t == Type::FLOAT64)
        return 8;

    return 0;
}

typedef std::function<void(void *dest, const char *src, bool be)> cast_t;

void addElementDefinition(const std::vector<std::string> &tokens, std::vector<PlyElement> &elementDefinitions)
{
    FN("addElementDefinition");

    assert(std::string(tokens.at(0)) == "element");
    std::size_t elementCount = std::stoul(tokens.at(2));
    elementDefinitions.emplace_back(tokens.at(1), elementCount);
}

void addProperty(const std::vector<std::string> &tokens, PlyElement &elementDefinition)
{
    FN("addProperty");

    auto &properties = elementDefinition.properties;
    if (std::string(tokens.at(1)) == "list")
    {
        properties.emplace_back(property_type_from_string(tokens.at(2)), property_type_from_string(tokens.at(3)), tokens.back());
    }
    else
    {
        properties.emplace_back(property_type_from_string(tokens.at(1)), tokens.back());
    }
}

ModelLoader_ply::ModelLoader_ply() { FN("ModelLoader_ply::ModelLoader_ply"); }

ModelLoader_ply::~ModelLoader_ply() { FN("ModelLoader_ply::~ModelLoader_ply"); }

void ModelLoader_ply::ImportMesh(const char *inputData, Mesh *outputMesh)
{
    FN("ModelLoader_ply::ImportMesh");
    OPTICK_EVENT();

    SubMesh *currentSubMesh = nullptr;

    currentSubMesh = outputMesh->CreateSubMesh();
    currentSubMesh->m_startIndex = 0;

    Format format = Format::INVALID;

    std::string line;
    std::istringstream dataStream(inputData);

    safeGetline(dataStream, line);
    if (line != "ply")
    {
        LOG_ERROR("Invalid file format.");
        return;
    }

    safeGetline(dataStream, line);
    // Read file format.
    if (line == "format ascii 1.0")
    {
        format = Format::ASCII;
    }
    else if (line == "format binary_little_endian 1.0")
    {
        format = Format::BINARY_LITTLE_ENDIAN;
    }
    else if (line == "format binary_big_endian 1.0")
    {
        format = Format::BINARY_BIG_ENDIAN;
    }
    else
    {
        LOG_ERROR("Unsupported PLY format : %s", line);
        return;
    }

    std::vector<PlyElement> elements;

    while (safeGetline(dataStream, line))
    {
        std::vector<std::string> tokens;
        tokens.clear();

        auto begin = line.begin();
        const auto end = line.end();
        auto eot = begin;
        while (eot != end)
        {
            // Skip all delimiters.
            while (begin != end && *begin == ' ')
            {
                ++begin;
            }
            eot = std::find(begin, end, ' ');

            tokens.emplace_back(begin, eot);
            if (eot != end)
            {
                // Move begin after delimiter.
                begin = eot + 1;
            }
        }

        if (std::string(tokens.at(0)) != "end_header")
        {
            const std::string lineType = tokens.at(0);
            if (lineType == "element")
            {
                addElementDefinition(tokens, elements);
                continue;
            }
            else if (lineType == "property")
            {
                addProperty(tokens, elements.back());
                continue;
            }
            if (lineType == "comment")
            {
                continue;
            }
            else
            {
                LOG_ERROR("Invalid header line.");
                return;
            }
        }
        else
        {
            break;
        }
    }

    if (format == Format::BINARY_BIG_ENDIAN || format == Format::BINARY_LITTLE_ENDIAN)
    {
        readDataBinary(format, elements, dataStream, inputData, outputMesh);
    }
    else if (format == Format::ASCII)
    {
        readDataASCII(format, elements, dataStream, outputMesh);
    }

    if (currentSubMesh == nullptr)
    {
        currentSubMesh = outputMesh->CreateSubMesh();
        currentSubMesh->m_startIndex = 0;
    }

    if (currentSubMesh != nullptr)
    {
        currentSubMesh->m_numIndices = outputMesh->m_indices.size() - currentSubMesh->m_startIndex;
    }
}

void ModelLoader_ply::readDataBinary(Format format, std::vector<PlyElement> &elements, std::istringstream &dataStream, const char *inputData, Mesh *outputMesh)
{
    FN("ModelLoader_ply::readDataBinary");

    int currIndex = dataStream.tellg();
    dataStream.seekg(0, std::ios::end);
    int endIndex = dataStream.tellg();

    for (int elementIndex = 0; elementIndex < elements.size(); elementIndex++)
    {
        PlyElement &element = elements.at(elementIndex);
        if (element.name == "vertex")
        {
            for (int vertexIndex = 0; vertexIndex < element.size; vertexIndex++)
            {
                for (int propertyIndex = 0; propertyIndex < element.properties.size(); propertyIndex++)
                {
                    PlyProperty &prop = element.properties.at(propertyIndex);

                    size_t sizeInBytes = property_size_from_type(prop.propertyType);
                    float value = 0.0f;

                    if (prop.propertyType == Type::FLOAT32)
                    {
                        memcpy(&value, inputData + currIndex, sizeInBytes);
                    }
                    else if (prop.propertyType == Type::FLOAT64)
                    {
                        float value_temp = 0.0f;
                        memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                        if (format == Format::BINARY_BIG_ENDIAN)
                            value_temp = endian_swap<uint64_t, float>(value_temp);

                        value = (float)value_temp;
                    }
                    else if (prop.propertyType == Type::INT16)
                    {
                        short value_temp = 0;
                        memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                        if (format == Format::BINARY_BIG_ENDIAN)
                            value_temp = endian_swap<int16_t, int16_t>(value_temp);

                        value = (float)value_temp;
                    }
                    else if (prop.propertyType == Type::INT32)
                    {
                        int value_temp = 0;
                        memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                        if (format == Format::BINARY_BIG_ENDIAN)
                            value_temp = endian_swap<int32_t, int32_t>(value_temp);

                        value = (float)value_temp;
                    }
                    else if (prop.propertyType == Type::INT8)
                    {
                        char value_temp = 0;
                        memcpy(&value_temp, inputData + currIndex, sizeInBytes);
                        value = (float)value_temp;
                    }
                    else if (prop.propertyType == Type::UINT16)
                    {
                        uint16_t value_temp = 0;
                        memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                        if (format == Format::BINARY_BIG_ENDIAN)
                            value_temp = endian_swap<uint16_t, uint16_t>(value_temp);

                        value = (float)value_temp;
                    }
                    else if (prop.propertyType == Type::UINT32)
                    {
                        uint32_t value_temp = 0;
                        memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                        if (format == Format::BINARY_BIG_ENDIAN)
                            value_temp = endian_swap<uint32_t, uint32_t>(value_temp);

                        value = (float)value_temp;
                    }
                    else if (prop.propertyType == Type::UINT8)
                    {
                        uint8_t value_temp = 0;
                        memcpy(&value_temp, inputData + currIndex, sizeInBytes);
                        value = (float)value_temp;
                    }

                    if (prop.name == "x" || prop.name == "y" || prop.name == "z")
                    {
                        while (outputMesh->m_vertices.size() <= vertexIndex)
                        {
                            outputMesh->m_vertices.push_back(Vector3<float>());
                        }

                        if (prop.name == "x")
                        {
                            outputMesh->m_vertices[vertexIndex].x = value;
                        }

                        if (prop.name == "y")
                        {
                            outputMesh->m_vertices[vertexIndex].y = value;
                        }

                        if (prop.name == "z")
                        {
                            outputMesh->m_vertices[vertexIndex].z = value;
                        }
                    }

                    if (prop.name == "nx" || prop.name == "ny" || prop.name == "nz")
                    {
                        while (outputMesh->m_normals.size() <= vertexIndex)
                        {
                            outputMesh->m_normals.push_back(Vector3<float>());
                        }

                        if (prop.name == "nx")
                        {
                            outputMesh->m_normals[vertexIndex].x = value;
                        }

                        if (prop.name == "ny")
                        {
                            outputMesh->m_normals[vertexIndex].y = value;
                        }

                        if (prop.name == "nz")
                        {
                            outputMesh->m_normals[vertexIndex].z = value;
                        }
                    }

                    if (prop.name == "u" || prop.name == "v")
                    {
                        while (outputMesh->m_texCoords.size() <= vertexIndex)
                        {
                            outputMesh->m_texCoords.push_back(Vector2<float>());
                        }

                        if (prop.name == "u")
                        {
                            outputMesh->m_texCoords[vertexIndex].x = value;
                        }

                        if (prop.name == "v")
                        {
                            outputMesh->m_texCoords[vertexIndex].y = value;
                        }
                    }
                    currIndex += sizeInBytes;
                }
            }
        }
        else if (element.name == "face")
        {
            for (int indexIndex = 0; indexIndex < element.size; indexIndex++)
            {
                for (int propertyIndex = 0; propertyIndex < element.properties.size(); propertyIndex++)
                {
                    PlyProperty &prop = element.properties.at(propertyIndex);

                    if (prop.isList)
                    {
                        uint32_t count = 0;
                        if (prop.listType == Type::INT16)
                        {
                            short value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<int16_t, int16_t>(value_temp);

                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::INT32)
                        {
                            int value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<int32_t, int32_t>(value_temp);

                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::INT8)
                        {
                            char value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));
                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::UINT16)
                        {
                            uint16_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<uint16_t, uint16_t>(value_temp);

                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::UINT32)
                        {
                            uint32_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<uint32_t, uint32_t>(value_temp);

                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::UINT8)
                        {
                            uint8_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));
                            count = (uint32_t)value_temp;
                        }
                        currIndex += property_size_from_type(prop.listType);

                        for (size_t i = 0; i < count; i++)
                        {
                            size_t sizeInBytes = property_size_from_type(prop.propertyType);
                            uint32_t value = 0;

                            if (prop.propertyType == Type::FLOAT32)
                            {
                                memcpy(&value, inputData + currIndex, sizeInBytes);
                            }
                            else if (prop.propertyType == Type::FLOAT64)
                            {
                                float value_temp = 0.0f;
                                memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                                if (format == Format::BINARY_BIG_ENDIAN)
                                    value_temp = endian_swap<uint64_t, float>(value_temp);

                                value = (uint32_t)value_temp;
                            }
                            else if (prop.propertyType == Type::INT16)
                            {
                                short value_temp = 0;
                                memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                                if (format == Format::BINARY_BIG_ENDIAN)
                                    value_temp = endian_swap<int16_t, int16_t>(value_temp);

                                value = (uint32_t)value_temp;
                            }
                            else if (prop.propertyType == Type::INT32)
                            {
                                int value_temp = 0;
                                memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                                if (format == Format::BINARY_BIG_ENDIAN)
                                    value_temp = endian_swap<int32_t, int32_t>(value_temp);

                                value = (uint32_t)value_temp;
                            }
                            else if (prop.propertyType == Type::INT8)
                            {
                                char value_temp = 0;
                                memcpy(&value_temp, inputData + currIndex, sizeInBytes);
                                value = (uint32_t)value_temp;
                            }
                            else if (prop.propertyType == Type::UINT16)
                            {
                                uint16_t value_temp = 0;
                                memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                                if (format == Format::BINARY_BIG_ENDIAN)
                                    value_temp = endian_swap<uint16_t, uint16_t>(value_temp);

                                value = (uint32_t)value_temp;
                            }
                            else if (prop.propertyType == Type::UINT32)
                            {
                                uint32_t value_temp = 0;
                                memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                                if (format == Format::BINARY_BIG_ENDIAN)
                                    value_temp = endian_swap<uint32_t, uint32_t>(value_temp);

                                value = (uint32_t)value_temp;
                            }
                            else if (prop.propertyType == Type::UINT8)
                            {
                                uint8_t value_temp = 0;
                                memcpy(&value_temp, inputData + currIndex, sizeInBytes);
                                value = (uint32_t)value_temp;
                            }

                            outputMesh->m_indices.push_back(value);
                            currIndex += sizeInBytes;
                        }
                    }
                    else
                    {
                        size_t sizeInBytes = property_size_from_type(prop.propertyType);
                        uint32_t value = 0;

                        if (prop.propertyType == Type::FLOAT32)
                        {
                            memcpy(&value, inputData + currIndex, sizeInBytes);
                        }
                        else if (prop.propertyType == Type::FLOAT64)
                        {
                            float value_temp = 0.0f;
                            memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<uint64_t, float>(value_temp);

                            value = (uint32_t)value_temp;
                        }
                        else if (prop.propertyType == Type::INT16)
                        {
                            short value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<int16_t, int16_t>(value_temp);

                            value = (uint32_t)value_temp;
                        }
                        else if (prop.propertyType == Type::INT32)
                        {
                            int value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<int32_t, int32_t>(value_temp);

                            value = (uint32_t)value_temp;
                        }
                        else if (prop.propertyType == Type::INT8)
                        {
                            char value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, sizeInBytes);
                            value = (uint32_t)value_temp;
                        }
                        else if (prop.propertyType == Type::UINT16)
                        {
                            uint16_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<uint16_t, uint16_t>(value_temp);

                            value = (uint32_t)value_temp;
                        }
                        else if (prop.propertyType == Type::UINT32)
                        {
                            uint32_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, sizeInBytes);

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<uint32_t, uint32_t>(value_temp);

                            value = (uint32_t)value_temp;
                        }
                        else if (prop.propertyType == Type::UINT8)
                        {
                            uint8_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, sizeInBytes);
                            value = (uint32_t)value_temp;
                        }

                        outputMesh->m_indices.push_back(value);
                        currIndex += sizeInBytes;
                    }
                }
            }
        }
        else
        {
            for (int indexIndex = 0; indexIndex < element.size; indexIndex++)
            {
                for (int propertyIndex = 0; propertyIndex < element.properties.size(); propertyIndex++)
                {
                    PlyProperty &prop = element.properties.at(propertyIndex);

                    if (prop.isList)
                    {
                        uint32_t count = 0;
                        if (prop.listType == Type::INT16)
                        {
                            short value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<int16_t, int16_t>(value_temp);

                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::INT32)
                        {
                            int value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<int32_t, int32_t>(value_temp);

                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::INT8)
                        {
                            char value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));
                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::UINT16)
                        {
                            uint16_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<uint16_t, uint16_t>(value_temp);

                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::UINT32)
                        {
                            uint32_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));

                            if (format == Format::BINARY_BIG_ENDIAN)
                                value_temp = endian_swap<uint32_t, uint32_t>(value_temp);

                            count = (uint32_t)value_temp;
                        }
                        else if (prop.listType == Type::UINT8)
                        {
                            uint8_t value_temp = 0;
                            memcpy(&value_temp, inputData + currIndex, property_size_from_type(prop.listType));
                            count = (uint32_t)value_temp;
                        }
                        currIndex += property_size_from_type(prop.listType);

                        for (size_t i = 0; i < count; i++)
                        {
                            size_t sizeInBytes = property_size_from_type(prop.propertyType);
                            currIndex += sizeInBytes;
                        }
                    }
                    else
                    {
                        size_t sizeInBytes = property_size_from_type(prop.propertyType);
                        currIndex += sizeInBytes;
                    }
                }
            }
        }
    }
}

void ModelLoader_ply::readDataASCII(Format format, std::vector<PlyElement> &elements, std::istringstream &dataStream, Mesh *outputMesh)
{
    FN("ModelLoader_ply::readDataASCII");

    std::vector<std::string> dataStringArray;
    dataStringArray.clear();

    std::string line;
    while (safeGetline(dataStream, line))
    {
        auto begin = line.begin();
        const auto end = line.end();
        auto eot = begin;
        while (eot != end)
        {
            // Skip all delimiters.
            while (begin != end && (*begin == ' '))
            {
                ++begin;
            }
            eot = std::find(begin, end, ' ');

            std::string value = std::string(begin, eot);
            if (value.size() > 0)
            {
                dataStringArray.emplace_back(value);
            }

            if (eot != end)
            {
                // Move begin after delimiter.
                begin = eot + 1;
            }
        }
    }

    int currIndex = 0;
    for (int elementIndex = 0; elementIndex < elements.size(); elementIndex++)
    {
        PlyElement &element = elements.at(elementIndex);
        if (element.name == "vertex")
        {
            for (int vertexIndex = 0; vertexIndex < element.size; vertexIndex++)
            {
                for (int propertyIndex = 0; propertyIndex < element.properties.size(); propertyIndex++)
                {
                    PlyProperty &prop = element.properties.at(propertyIndex);

                    float value = 0.0f;
                    std::string valueString = dataStringArray[currIndex++];

                    if (prop.propertyType == Type::FLOAT32 || prop.propertyType == Type::FLOAT64)
                    {
                        value = (float)std::stoi(valueString);
                    }
                    else if (prop.propertyType == Type::UINT8 || prop.propertyType == Type::INT8 || prop.propertyType == Type::UINT16 || prop.propertyType == Type::INT16 || prop.propertyType == Type::UINT32 || prop.propertyType == Type::INT32)
                    {
                        value = (float)std::stod(valueString);
                    }

                    if (prop.name == "x" || prop.name == "y" || prop.name == "z")
                    {
                        while (outputMesh->m_vertices.size() <= vertexIndex)
                        {
                            outputMesh->m_vertices.push_back(Vector3<float>());
                        }

                        if (prop.name == "x")
                        {
                            outputMesh->m_vertices[vertexIndex].x = value;
                        }

                        if (prop.name == "y")
                        {
                            outputMesh->m_vertices[vertexIndex].y = value;
                        }

                        if (prop.name == "z")
                        {
                            outputMesh->m_vertices[vertexIndex].z = value;
                        }
                    }

                    if (prop.name == "nx" || prop.name == "ny" || prop.name == "nz")
                    {
                        while (outputMesh->m_normals.size() <= vertexIndex)
                        {
                            outputMesh->m_normals.push_back(Vector3<float>());
                        }

                        if (prop.name == "nx")
                        {
                            outputMesh->m_normals[vertexIndex].x = value;
                        }

                        if (prop.name == "ny")
                        {
                            outputMesh->m_normals[vertexIndex].y = value;
                        }

                        if (prop.name == "nz")
                        {
                            outputMesh->m_normals[vertexIndex].z = value;
                        }
                    }

                    if (prop.name == "u" || prop.name == "v")
                    {
                        while (outputMesh->m_texCoords.size() <= vertexIndex)
                        {
                            outputMesh->m_texCoords.push_back(Vector2<float>());
                        }

                        if (prop.name == "u")
                        {
                            outputMesh->m_texCoords[vertexIndex].x = value;
                        }

                        if (prop.name == "v")
                        {
                            outputMesh->m_texCoords[vertexIndex].y = value;
                        }
                    }
                }
            }
        }
        else if (element.name == "face")
        {
            for (int indexIndex = 0; indexIndex < element.size; indexIndex++)
            {
                for (int propertyIndex = 0; propertyIndex < element.properties.size(); propertyIndex++)
                {
                    PlyProperty &prop = element.properties.at(propertyIndex);

                    if (prop.isList)
                    {
                        uint32_t count = 0;
                        std::string valueString = dataStringArray[currIndex++];

                        if (prop.listType == Type::FLOAT32 || prop.listType == Type::FLOAT64)
                        {
                            count = (float)std::stoi(valueString);
                        }
                        else if (prop.listType == Type::UINT8 || prop.listType == Type::INT8 || prop.listType == Type::UINT16 || prop.listType == Type::INT16 || prop.listType == Type::UINT32 || prop.listType == Type::INT32)
                        {
                            count = (float)std::stod(valueString);
                        }

                        for (size_t i = 0; i < count; i++)
                        {
                            uint32_t value = 0;
                            std::string valueString = dataStringArray[currIndex++];

                            if (prop.propertyType == Type::FLOAT32 || prop.propertyType == Type::FLOAT64)
                            {
                                value = (uint32_t)std::stoi(valueString);
                            }
                            else if (prop.propertyType == Type::UINT8 || prop.propertyType == Type::INT8 || prop.propertyType == Type::UINT16 || prop.propertyType == Type::INT16 || prop.propertyType == Type::UINT32 ||
                                     prop.propertyType == Type::INT32)
                            {
                                value = (uint32_t)std::stod(valueString);
                            }

                            outputMesh->m_indices.push_back(value);
                        }
                    }
                    else
                    {
                        uint32_t value = 0;
                        std::string valueString = dataStringArray[currIndex++];

                        if (prop.propertyType == Type::FLOAT32 || prop.propertyType == Type::FLOAT64)
                        {
                            value = (uint32_t)std::stoi(valueString);
                        }
                        else if (prop.propertyType == Type::UINT8 || prop.propertyType == Type::INT8 || prop.propertyType == Type::UINT16 || prop.propertyType == Type::INT16 || prop.propertyType == Type::UINT32 || prop.propertyType == Type::INT32)
                        {
                            value = (uint32_t)std::stod(valueString);
                        }

                        outputMesh->m_indices.push_back(value);
                    }
                }
            }
        }
        else
        {
            for (int indexIndex = 0; indexIndex < element.size; indexIndex++)
            {
                for (int propertyIndex = 0; propertyIndex < element.properties.size(); propertyIndex++)
                {
                    PlyProperty &prop = element.properties.at(propertyIndex);
                    if (prop.isList)
                    {
                        uint32_t count = 0;
                        std::string valueString = dataStringArray[currIndex++];
                        if (prop.listType == Type::FLOAT32 || prop.listType == Type::FLOAT64)
                        {
                            count = (float)std::stoi(valueString);
                        }
                        else if (prop.listType == Type::UINT8 || prop.listType == Type::INT8 || prop.listType == Type::UINT16 || prop.listType == Type::INT16 || prop.listType == Type::UINT32 || prop.listType == Type::INT32)
                        {
                            count = (float)std::stod(valueString);
                        }

                        for (size_t i = 0; i < count; i++)
                        {
                            uint32_t value = 0;
                            std::string valueString = dataStringArray[currIndex++];
                            // Just ignore the result
                        }
                    }
                    else
                    {
                        uint32_t value = 0;
                        std::string valueString = dataStringArray[currIndex++];
                        // Just ignore the result
                    }
                }
            }
        }
    }
    uint32_t i = 0;
    return;
}

// See
// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
std::istream &ModelLoader_ply::safeGetline(std::istream &is, std::string &t)
{
    FN("ModelLoader_ply::safeGetline");

    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf *sb = is.rdbuf();

    if (se)
    {
        for (;;)
        {
            int c = sb->sbumpc();
            switch (c)
            {
            case '\n':
                return is;
            case '\r':
                if (sb->sgetc() == '\n')
                    sb->sbumpc();
                return is;
            case EOF:
                // Also handle the case when the last line has no line ending
                if (t.empty())
                    is.setstate(std::ios::eofbit);
                return is;
            default:
                t += static_cast<char>(c);
            }
        }
    }

    return is;
}
} // namespace bow
