#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xGlslAdapter.h>

#include <CoreSystems/BowLogger.h>

#include <cctype>
#include <regex>

namespace bow
{
namespace
{

//! Name of the block instance a push-constant declaration ends with.
/*!
    Scans forward from the opening brace to the matching closing brace and
    reads the identifier that follows it, i.e. the "pushConstants" in
    `layout(push_constant) uniform PushConstants { ... } pushConstants;`.

    \param source Full shader source.
    \param bracePos Index of the block's opening brace.
    \return The instance name, or an empty string when the block is anonymous
            or unterminated.
*/
std::string ReadInstanceName(const std::string &source, size_t bracePos)
{
    int depth = 0;
    size_t i = bracePos;
    for (; i < source.size(); ++i)
    {
        if (source[i] == '{')
        {
            ++depth;
        }
        else if (source[i] == '}')
        {
            --depth;
            if (depth == 0)
            {
                ++i;
                break;
            }
        }
    }

    while (i < source.size() && std::isspace(static_cast<unsigned char>(source[i])))
    {
        ++i;
    }

    const size_t start = i;
    while (i < source.size() && (std::isalnum(static_cast<unsigned char>(source[i])) || source[i] == '_'))
    {
        ++i;
    }

    return source.substr(start, i - start);
}

} // namespace

GlslAdapter::Result GlslAdapter::Adapt(const std::string &source)
{
    FN("GlslAdapter::Adapt");

    Result result;
    result.source = source;

    // layout(set = 0, binding = 1, rgba8) -> layout(binding = 1, rgba8)
    // Also covers the spacing-free spelling the examples mix in.
    result.source = std::regex_replace(result.source, std::regex(R"(\bset\s*=\s*\d+\s*,\s*)"), "");

    // A trailing set qualifier, layout(binding = 0, set = 1), leaves the comma
    // in front of it behind instead.
    result.source = std::regex_replace(result.source, std::regex(R"(\s*,\s*set\s*=\s*\d+)"), "");

    // layout(push_constant) uniform Block { ... } instance;
    //   -> layout(std140) uniform Block { ... } instance;
    //
    // std140 is spelled out because a push-constant block carries no packing
    // qualifier of its own and the default layout is implementation defined,
    // which would make the offsets a caller writes at unpredictable.
    const std::regex pushConstant(R"(layout\s*\(\s*push_constant\s*\)\s*uniform\s+(\w+)\s*\{)");

    std::smatch match;
    std::string::const_iterator searchStart = result.source.cbegin();
    size_t searchOffset = 0;

    while (std::regex_search(searchStart, result.source.cend(), match, pushConstant))
    {
        const size_t matchPos = searchOffset + match.position(0);
        const std::string blockName = match[1].str();
        const size_t bracePos = matchPos + match.length(0) - 1;

        const std::string instanceName = ReadInstanceName(result.source, bracePos);
        if (!instanceName.empty())
        {
            result.pushConstantBlocks[instanceName] = blockName;
        }
        else
        {
            LOG_WARNING("Push constant block '%s' has no instance name; it cannot be addressed from the API.", blockName.c_str());
        }

        const std::string replacement = "layout(std140) uniform " + blockName + " {";
        result.source.replace(matchPos, match.length(0), replacement);

        searchOffset = matchPos + replacement.size();
        searchStart = result.source.cbegin() + searchOffset;
    }

    return result;
}

} // namespace bow
