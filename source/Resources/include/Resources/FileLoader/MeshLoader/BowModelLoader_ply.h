#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "CoreSystems/BowMath.h"

#include "Resources/Resources/BowMaterial.h"
#include "Resources/Resources/BowMesh.h"

namespace bow
{

enum class Type : uint8_t
{
    INVALID,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    FLOAT32,
    FLOAT64
};

enum class Format
{
    INVALID,
    ASCII,
    BINARY_LITTLE_ENDIAN,
    BINARY_BIG_ENDIAN
};

struct PlyProperty
{
    PlyProperty(Type type, const std::string &_name) : name(_name), propertyType(type) {}
    PlyProperty(Type list_type, Type prop_type, const std::string &_name) : name(_name), propertyType(prop_type), isList(true), listType(list_type) {}

    std::string name;
    Type propertyType;
    bool isList{false};
    Type listType{Type::INVALID};
    size_t listCount{0};
};

struct PlyElement
{
    PlyElement(const std::string &_name, size_t count) : name(_name), size(count) {}

    std::string name;
    size_t size;
    std::vector<PlyProperty> properties;
};

struct PropertyInfo
{
    int stride;
    std::string str;
};

static std::map<Type, PropertyInfo> PropertyTable{{Type::INT8, {1, "char"}},   {Type::UINT8, {1, "uchar"}},   {Type::INT16, {2, "short"}},   {Type::UINT16, {2, "ushort"}},  {Type::INT32, {4, "int"}},
                                                  {Type::UINT32, {4, "uint"}}, {Type::FLOAT32, {4, "float"}}, {Type::FLOAT64, {8, "float"}}, {Type::INVALID, {0, "INVALID"}}};

// ---------------------------------------------------------------------------
/** @brief A mesh represents geometry without any material.
 */
class ModelLoader_ply
{
  public:
    ModelLoader_ply();
    ~ModelLoader_ply();

    /** Imports Mesh and (optionally) Material data from a .obj file.
    @remarks
    This method imports data from loaded data opened from a .obj file and places
    it's contents into the Mesh object which is passed in.
    @param inputData The Data holding the mesh.
    @param outputMesh Pointer to the Mesh object which will receive the data.
    Should be blank already.
    */
    void ImportMesh(const char *inputData, Mesh *outputMesh);

  private:
    // you shall not copy
    ModelLoader_ply(const ModelLoader_ply &other) = delete;
    ModelLoader_ply &operator=(const ModelLoader_ply &other) = delete;

    std::istream &safeGetline(std::istream &is, std::string &t);

    void readDataBinary(Format format, std::vector<PlyElement> &elements, std::istringstream &dataStream, const char *inputData, Mesh *outputMesh);
    void readDataASCII(Format format, std::vector<PlyElement> &elements, std::istringstream &dataStream, Mesh *outputMesh);
};
} // namespace bow