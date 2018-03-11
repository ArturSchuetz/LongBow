#include "BowResources.h"
#include "BowModelLoader_obj.h"

#include <sstream>
#include <iostream>

namespace bow {

	#define IS_SPACE(x) (((x) == ' ') || ((x) == '\t'))
	#define IS_DIGIT(x) (static_cast<unsigned int>((x) - '0') < static_cast<unsigned int>(10))
	#define IS_NEW_LINE(x) (((x) == '\r') || ((x) == '\n') || ((x) == '\0'))

	ModelLoader_obj::ModelLoader_obj()
	{

	}

	ModelLoader_obj::~ModelLoader_obj()
	{        

	}

	void ModelLoader_obj::ImportMesh(const char* inputData, Mesh* outputMesh)
	{
		SubMesh* currentSubMesh = nullptr;

		std::string line;
		std::istringstream dataStream(inputData);
		while (safeGetline(dataStream, line)) 
		{
			// Trim newline '\r\n' or '\n'
			if (line.size() > 0) {
				if (line[line.size() - 1] == '\n')
					line.erase(line.size() - 1);
			}

			if (line.size() > 0) {
				if (line[line.size() - 1] == '\r')
					line.erase(line.size() - 1);
			}

			// Skip if empty line.
			if (line.empty()) {
				continue;
			}

			// Skip leading space.
			const char *token = line.c_str();
			token += strspn(token, " \t");

			// vertex
			if (token[0] == 'v' && IS_SPACE((token[1]))) {
				token += 2;
				float x, y, z;
				parseReal3(&x, &y, &z, &token);
				vertices.push_back(Vector3<float>(x, y, z));
				continue;
			}

			// normal
			if (token[0] == 'v' && token[1] == 'n' && IS_SPACE((token[2]))) {
				token += 3;
				float x, y, z;
				parseReal3(&x, &y, &z, &token);
				normals.push_back(Vector3<float>(x, y, z));
				continue;
			}

			// texcoord
			if (token[0] == 'v' && token[1] == 't' && IS_SPACE((token[2]))) {
				token += 3;
				float x, y;
				parseReal2(&x, &y, &token);
				texCoords.push_back(Vector2<float>(x, y));
				continue;
			}

			// face
			if (token[0] == 'f' && IS_SPACE((token[1]))) 
			{
				token += 2;
				token += strspn(token, " \t");

				std::vector<vertex_index> face;
				face.reserve(3);

				while (!IS_NEW_LINE(token[0])) 
				{
					vertex_index vi;
					if (!parseTriple(&token, static_cast<int>(vertices.size()), static_cast<int>(normals.size()), static_cast<int>(texCoords.size()), &vi))
					{
						LOG_ERROR("Failed parse `f' line(e.g. zero value for face index).");
						return;
					}

					face.push_back(vi);
					size_t n = strspn(token, " \t\r");
					token += n;
				}

				if (!exportFaceGroupToShape(outputMesh, face))
				{
					LOG_ERROR("Failed to add faces to submesh");
					return;
				}

				continue;
			}

			// use mtl
			if ((0 == strncmp(token, "usemtl", 6)) && IS_SPACE((token[6]))) 
			{
				token += 7;
				std::stringstream ss;
				ss << token;
				std::string namebuf = ss.str();

				if (currentSubMesh == nullptr)
				{
					currentSubMesh = outputMesh->CreateSubMesh();
					currentSubMesh->m_startIndex = outputMesh->m_indices.size();
				}

				currentSubMesh->m_material = namebuf;

				continue;
			}

			// load mtl
			if ((0 == strncmp(token, "mtllib", 6)) && IS_SPACE((token[6]))) 
			{
				token += 7;
				std::stringstream ss;
				ss.str(std::string(token));
				std::string item;

				while (std::getline(ss, item, ' ')) {
					std::size_t foundPos = outputMesh->m_name.find_last_of("/");
					std::string filePath = "";
					if (foundPos >= 0)
					{
						filePath = outputMesh->m_name.substr(0, foundPos+1);
					}
					outputMesh->m_materialFilesList.push_back(filePath + item);
				}

				if (outputMesh->m_materialFilesList.empty()) {
					LOG_WARNING("Looks like empty filename for mtllib. Use default material->");
				}

				continue;
			}

			// group name
			if (token[0] == 'g' && IS_SPACE((token[1])))
			{
				if (currentSubMesh != nullptr)
				{
					currentSubMesh->m_numIndices = outputMesh->m_indices.size() - currentSubMesh->m_startIndex;
				}

				std::vector<std::string> names;
				names.reserve(2);

				while (!IS_NEW_LINE(token[0]))
				{
					std::string str = parseString(&token);
					names.push_back(str);
					token += strspn(token, " \t\r");  // skip tag
				}

				LOG_ASSERT(names.size() > 0, "No Groupe Name");

				// names[0] must be 'g', so skip the 0th element.
				if (names.size() > 1)
				{
					currentSubMesh = outputMesh->CreateSubMesh(names[1]);
				}
				else
				{
					currentSubMesh = outputMesh->CreateSubMesh();
				}
				currentSubMesh->m_startIndex = outputMesh->m_indices.size();

				continue;
			}

			// Ignore unknown command.
		}

		if (currentSubMesh != nullptr)
		{
			currentSubMesh->m_numIndices = outputMesh->m_indices.size() - currentSubMesh->m_startIndex;
		}
	}

	void ModelLoader_obj::ImportMaterial(const char* inputData, MaterialCollection* outputMaterial)
	{
		Material* material = nullptr;

		// Issue 43. `d` wins against `Tr` since `Tr` is not in the MTL specification.
		bool has_d = false;
		bool has_tr = false;

		std::string linebuf;
		std::istringstream dataStream(inputData);
		while (safeGetline(dataStream, linebuf))
		{
			// Trim trailing whitespace.
			if (linebuf.size() > 0) {
				linebuf = linebuf.substr(0, linebuf.find_last_not_of(" \t") + 1);
			}

			// Trim newline '\r\n' or '\n'
			if (linebuf.size() > 0) {
				if (linebuf[linebuf.size() - 1] == '\n')
					linebuf.erase(linebuf.size() - 1);
			}
			if (linebuf.size() > 0) {
				if (linebuf[linebuf.size() - 1] == '\r')
					linebuf.erase(linebuf.size() - 1);
			}

			// Skip if empty line.
			if (linebuf.empty()) {
				continue;
			}

			// Skip leading space.
			const char *token = linebuf.c_str();
			token += strspn(token, " \t");

			assert(token);
			if (token[0] == '\0') continue;  // empty line

			if (token[0] == '#') continue;  // comment line

											// new mtl
			if ((0 == strncmp(token, "newmtl", 6)) && IS_SPACE((token[6]))) {

				has_d = false;
				has_tr = false;

				// set new mtl name
				token += 7;
				{
					std::stringstream sstr;
					sstr << token;
					std::string name = sstr.str();

					if (name.size() > 1)
					{
						material = outputMaterial->CreateMaterial(name);
						material->name = name;
					}
					else
					{
						material = outputMaterial->CreateMaterial();
					}
				}

				continue;
			}

			// ambient
			if (token[0] == 'K' && token[1] == 'a' && IS_SPACE((token[2]))) {
				token += 2;
				float r, g, b;
				parseReal3(&r, &g, &b, &token);
				material->ambient[0] = r;
				material->ambient[1] = g;
				material->ambient[2] = b;
				continue;
			}

			// diffuse
			if (token[0] == 'K' && token[1] == 'd' && IS_SPACE((token[2]))) {
				token += 2;
				float r, g, b;
				parseReal3(&r, &g, &b, &token);
				material->diffuse[0] = r;
				material->diffuse[1] = g;
				material->diffuse[2] = b;
				continue;
			}

			// specular
			if (token[0] == 'K' && token[1] == 's' && IS_SPACE((token[2]))) {
				token += 2;
				float r, g, b;
				parseReal3(&r, &g, &b, &token);
				material->specular[0] = r;
				material->specular[1] = g;
				material->specular[2] = b;
				continue;
			}

			// transmittance
			if ((token[0] == 'K' && token[1] == 't' && IS_SPACE((token[2]))) ||
				(token[0] == 'T' && token[1] == 'f' && IS_SPACE((token[2])))) {
				token += 2;
				float r, g, b;
				parseReal3(&r, &g, &b, &token);
				material->transmittance[0] = r;
				material->transmittance[1] = g;
				material->transmittance[2] = b;
				continue;
			}

			// ior(index of refraction)
			if (token[0] == 'N' && token[1] == 'i' && IS_SPACE((token[2]))) {
				token += 2;
				material->ior = parseReal(&token);
				continue;
			}

			// emission
			if (token[0] == 'K' && token[1] == 'e' && IS_SPACE(token[2])) {
				token += 2;
				float r, g, b;
				parseReal3(&r, &g, &b, &token);
				material->emission[0] = r;
				material->emission[1] = g;
				material->emission[2] = b;
				continue;
			}

			// shininess
			if (token[0] == 'N' && token[1] == 's' && IS_SPACE(token[2])) {
				token += 2;
				material->shininess = parseReal(&token);
				continue;
			}

			// illum model
			if (0 == strncmp(token, "illum", 5) && IS_SPACE(token[5])) {
				token += 6;
				material->illum = parseInt(&token);
				continue;
			}

			// dissolve
			if ((token[0] == 'd' && IS_SPACE(token[1]))) {
				token += 1;
				material->dissolve = parseReal(&token);

				if (has_tr) {

					LOG_WARNING(std::string("Both `d` and `Tr` parameters defined for \"" + material->name + "\". Use the value of `d` for dissolve.").c_str());
				}
				has_d = true;
				continue;
			}

			if (token[0] == 'T' && token[1] == 'r' && IS_SPACE(token[2])) {
				token += 2;
				if (has_d) {
					// `d` wins. Ignore `Tr` value.
					LOG_WARNING(std::string("Both `d` and `Tr` parameters defined for \"" + material->name + "\". Use the value of `d` for dissolve.").c_str());
				}
				else {
					// We invert value of Tr(assume Tr is in range [0, 1])
					// NOTE: Interpretation of Tr is application(exporter) dependent. For
					// some application(e.g. 3ds max obj exporter), Tr = d(Issue 43)
					material->dissolve = 1.0f - parseReal(&token);
				}
				has_tr = true;
				continue;
			}

			// PBR: roughness
			if (token[0] == 'P' && token[1] == 'r' && IS_SPACE(token[2])) {
				token += 2;
				material->roughness = parseReal(&token);
				continue;
			}

			// PBR: metallic
			if (token[0] == 'P' && token[1] == 'm' && IS_SPACE(token[2])) {
				token += 2;
				material->metallic = parseReal(&token);
				continue;
			}

			// PBR: sheen
			if (token[0] == 'P' && token[1] == 's' && IS_SPACE(token[2])) {
				token += 2;
				material->sheen = parseReal(&token);
				continue;
			}

			// PBR: clearcoat thickness
			if (token[0] == 'P' && token[1] == 'c' && IS_SPACE(token[2])) {
				token += 2;
				material->clearcoat_thickness = parseReal(&token);
				continue;
			}

			// PBR: clearcoat roughness
			if ((0 == strncmp(token, "Pcr", 3)) && IS_SPACE(token[3])) {
				token += 4;
				material->clearcoat_roughness = parseReal(&token);
				continue;
			}

			// PBR: anisotropy
			if ((0 == strncmp(token, "aniso", 5)) && IS_SPACE(token[5])) {
				token += 6;
				material->anisotropy = parseReal(&token);
				continue;
			}

			// PBR: anisotropy rotation
			if ((0 == strncmp(token, "anisor", 6)) && IS_SPACE(token[6])) {
				token += 7;
				material->anisotropy_rotation = parseReal(&token);
				continue;
			}

			// ambient texture
			if ((0 == strncmp(token, "map_Ka", 6)) && IS_SPACE(token[6])) {
				token += 7;
				parseTextureNameAndOption(&(material->ambient_texname), token, /* is_bump */ false);
				continue;
			}

			// diffuse texture
			if ((0 == strncmp(token, "map_Kd", 6)) && IS_SPACE(token[6])) {
				token += 7;
				parseTextureNameAndOption(&(material->diffuse_texname), token, /* is_bump */ false);
				continue;
			}

			// specular texture
			if ((0 == strncmp(token, "map_Ks", 6)) && IS_SPACE(token[6])) {
				token += 7;
				parseTextureNameAndOption(&(material->specular_texname), token, /* is_bump */ false);
				continue;
			}

			// specular highlight texture
			if ((0 == strncmp(token, "map_Ns", 6)) && IS_SPACE(token[6])) {
				token += 7;
				parseTextureNameAndOption(&(material->specular_highlight_texname), token, /* is_bump */ false);
				continue;
			}

			// bump texture
			if ((0 == strncmp(token, "map_bump", 8)) && IS_SPACE(token[8])) {
				token += 9;
				parseTextureNameAndOption(&(material->bump_texname), token, /* is_bump */ true);
				continue;
			}

			// bump texture
			if ((0 == strncmp(token, "map_Bump", 8)) && IS_SPACE(token[8])) {
				token += 9;
				parseTextureNameAndOption(&(material->bump_texname), token, /* is_bump */ true);
				continue;
			}

			// bump texture
			if ((0 == strncmp(token, "bump", 4)) && IS_SPACE(token[4])) {
				token += 5;
				parseTextureNameAndOption(&(material->bump_texname), token, /* is_bump */ true);
				continue;
			}

			// alpha texture
			if ((0 == strncmp(token, "map_d", 5)) && IS_SPACE(token[5])) {
				token += 6;
				material->alpha_texname = token;
				parseTextureNameAndOption(&(material->alpha_texname), token, /* is_bump */ false);
				continue;
			}

			// displacement texture
			if ((0 == strncmp(token, "disp", 4)) && IS_SPACE(token[4])) {
				token += 5;
				parseTextureNameAndOption(&(material->displacement_texname), token, /* is_bump */ false);
				continue;
			}

			// reflection map
			if ((0 == strncmp(token, "refl", 4)) && IS_SPACE(token[4])) {
				token += 5;
				parseTextureNameAndOption(&(material->reflection_texname), token, /* is_bump */ false);
				continue;
			}

			// PBR: roughness texture
			if ((0 == strncmp(token, "map_Pr", 6)) && IS_SPACE(token[6])) {
				token += 7;
				parseTextureNameAndOption(&(material->roughness_texname), token, /* is_bump */ false);
				continue;
			}

			// PBR: metallic texture
			if ((0 == strncmp(token, "map_Pm", 6)) && IS_SPACE(token[6])) {
				token += 7;
				parseTextureNameAndOption(&(material->metallic_texname), token, /* is_bump */ false);
				continue;
			}

			// PBR: sheen texture
			if ((0 == strncmp(token, "map_Ps", 6)) && IS_SPACE(token[6])) {
				token += 7;
				parseTextureNameAndOption(&(material->sheen_texname), token, /* is_bump */ false);
				continue;
			}

			// PBR: emissive texture
			if ((0 == strncmp(token, "map_Ke", 6)) && IS_SPACE(token[6])) {
				token += 7;
				parseTextureNameAndOption(&(material->emissive_texname), token, /* is_bump */ false);
				continue;
			}

			// PBR: normal map texture
			if ((0 == strncmp(token, "norm", 4)) && IS_SPACE(token[4])) {
				token += 5;
				parseTextureNameAndOption(&(material->normal_texname), token, /* is_bump */ false);  // @fixme { is_bump will be true? }
				continue;
			}

			// unknown parameter
			const char *_space = strchr(token, ' ');

			if (!_space) {
				_space = strchr(token, '\t');
			}

			if (_space) {
				std::ptrdiff_t len = _space - token;
				std::string key(token, static_cast<size_t>(len));
				std::string value = _space + 1;
				material->unknown_parameter.insert(std::pair<std::string, std::string>(key, value));
			}
		}
	}

	// ==========================================================
	// Private
	// ==========================================================

	bool ModelLoader_obj::exportFaceGroupToShape(Mesh* mesh, const std::vector<vertex_index> &face)
	{
		if (face.empty())
		{
			return false;
		}

		vertex_index i0 = face[0];
		vertex_index i1(-1);
		vertex_index i2 = face[1];

		size_t npolys = face.size();

		// Polygon -> triangle fan conversion
		for (size_t k = 2; k < npolys; k++) 
		{
			i1 = i2;
			i2 = face[k];

			index_t idx0, idx1, idx2;
			idx0.vertex_index = i0.v_idx;
			idx0.normal_index = i0.vn_idx;
			idx0.texcoord_index = i0.vt_idx;
			mesh->m_indices.push_back(getVertexIndex(mesh, idx0));

			idx1.vertex_index = i1.v_idx;
			idx1.normal_index = i1.vn_idx;
			idx1.texcoord_index = i1.vt_idx;
			mesh->m_indices.push_back(getVertexIndex(mesh, idx1));

			idx2.vertex_index = i2.v_idx;
			idx2.normal_index = i2.vn_idx;
			idx2.texcoord_index = i2.vt_idx;
			mesh->m_indices.push_back(getVertexIndex(mesh, idx2));
		}

		return true;
	}

	unsigned int ModelLoader_obj::getVertexIndex(Mesh* mesh, index_t index)
	{
		/*
		for (int i = mesh->m_vertices.size() - 1; i >= 0; --i)
		{
			if (mesh->m_vertices[i] == vertices[index.vertex_index]
				&& mesh->m_normals[i] == normals[index.normal_index]
				&& mesh->m_texCoords[i] == textureCoordinates[index.texcoord_index])
			{
				return i;
			}
		}
		*/

		mesh->m_vertices.push_back(vertices[index.vertex_index]);
		mesh->m_normals.push_back(normals[index.normal_index]);
		mesh->m_texCoords.push_back(texCoords[index.texcoord_index]);

		return mesh->m_vertices.size() - 1;
	}

	// See
	// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
	std::istream& ModelLoader_obj::safeGetline(std::istream &is, std::string &t) 
	{
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
					if (sb->sgetc() == '\n') sb->sbumpc();
					return is;
				case EOF:
					// Also handle the case when the last line has no line ending
					if (t.empty()) is.setstate(std::ios::eofbit);
					return is;
				default:
					t += static_cast<char>(c);
				}
			}
		}

		return is;
	}

	// Tries to parse a floating point number located at s.
	//
	// s_end should be a location in the string where reading should absolutely
	// stop. For example at the end of the string, to prevent buffer overflows.
	//
	// Parses the following EBNF grammar:
	//   sign    = "+" | "-" ;
	//   END     = ? anything not in digit ?
	//   digit   = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
	//   integer = [sign] , digit , {digit} ;
	//   decimal = integer , ["." , integer] ;
	//   float   = ( decimal , END ) | ( decimal , ("E" | "e") , integer , END ) ;
	//
	//  Valid strings are for example:
	//   -0  +3.1417e+2  -0.0E-3  1.0324  -1.41   11e2
	//
	// If the parsing is a success, result is set to the parsed value and true
	// is returned.
	//
	// The function is greedy and will parse until any of the following happens:
	//  - a non-conforming character is encountered.
	//  - s_end is reached.
	//
	// The following situations triggers a failure:
	//  - s >= s_end.
	//  - parse failure.
	//
	bool ModelLoader_obj::tryParseDouble(const char *s, const char *s_end, double *result)
	{
		if (s >= s_end)
		{
			return false;
		}

		double mantissa = 0.0;
		// This exponent is base 2 rather than 10.
		// However the exponent we parse is supposed to be one of ten,
		// thus we must take care to convert the exponent/and or the
		// mantissa to a * 2^E, where a is the mantissa and E is the
		// exponent.
		// To get the final double we will use ldexp, it requires the
		// exponent to be in base 2.
		int exponent = 0;

		// NOTE: THESE MUST BE DECLARED HERE SINCE WE ARE NOT ALLOWED
		// TO JUMP OVER DEFINITIONS.
		char sign = '+';
		char exp_sign = '+';
		char const *curr = s;

		// How many characters were read in a loop.
		int read = 0;
		// Tells whether a loop terminated due to reaching s_end.
		bool end_not_reached = false;

		/*
		BEGIN PARSING.
		*/

		// Find out what sign we've got.
		if (*curr == '+' || *curr == '-')
		{
			sign = *curr;
			curr++;
		}
		else if (IS_DIGIT(*curr))
		{
			/* Pass through. */
		}
		else
		{
			goto fail;
		}

		// Read the integer part.
		end_not_reached = (curr != s_end);
		while (end_not_reached && IS_DIGIT(*curr))
		{
			mantissa *= 10;
			mantissa += static_cast<int>(*curr - 0x30);
			curr++;
			read++;
			end_not_reached = (curr != s_end);
		}

		// We must make sure we actually got something.
		if (read == 0) goto fail;
		// We allow numbers of form "#", "###" etc.
		if (!end_not_reached) goto assemble;

		// Read the decimal part.
		if (*curr == '.')
		{
			curr++;
			read = 1;
			end_not_reached = (curr != s_end);
			while (end_not_reached && IS_DIGIT(*curr))
			{
				static const double pow_lut[] = {
					1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001,
				};
				const int lut_entries = sizeof pow_lut / sizeof pow_lut[0];

				// NOTE: Don't use powf here, it will absolutely murder precision.
				mantissa += static_cast<int>(*curr - 0x30) *
					(read < lut_entries ? pow_lut[read] : std::pow(10.0, -read));
				read++;
				curr++;
				end_not_reached = (curr != s_end);
			}
		}
		else if (*curr == 'e' || *curr == 'E')
		{

		}
		else
		{
			goto assemble;
		}

		if (!end_not_reached) goto assemble;

		// Read the exponent part.
		if (*curr == 'e' || *curr == 'E')
		{
			curr++;
			// Figure out if a sign is present and if it is.
			end_not_reached = (curr != s_end);
			if (end_not_reached && (*curr == '+' || *curr == '-'))
			{
				exp_sign = *curr;
				curr++;
			}
			else if (IS_DIGIT(*curr))
			{
				/* Pass through. */
			}
			else
			{
				// Empty E is not allowed.
				goto fail;
			}

			read = 0;
			end_not_reached = (curr != s_end);
			while (end_not_reached && IS_DIGIT(*curr))
			{
				exponent *= 10;
				exponent += static_cast<int>(*curr - 0x30);
				curr++;
				read++;
				end_not_reached = (curr != s_end);
			}
			exponent *= (exp_sign == '+' ? 1 : -1);
			if (read == 0) goto fail;
		}

	assemble:
		*result = (sign == '+' ? 1 : -1) * (exponent ? std::ldexp(mantissa * std::pow(5.0, exponent), exponent) : mantissa);
		return true;
	fail:
		return false;
	}

	float ModelLoader_obj::parseReal(const char **token, double default_value) 
	{
		(*token) += strspn((*token), " \t");
		const char *end = (*token) + strcspn((*token), " \t\r");
		double val = default_value;
		tryParseDouble((*token), end, &val);
		float f = static_cast<float>(val);
		(*token) = end;
		return f;
	}

	void ModelLoader_obj::parseReal2(float *x, float *y, const char **token,
		const double default_x,
		const double default_y) 
	{
		(*x) = parseReal(token, default_x);
		(*y) = parseReal(token, default_y);
	}

	void ModelLoader_obj::parseReal3(float *x, float *y, float *z,
		const char **token, const double default_x,
		const double default_y,
		const double default_z) 
	{
		(*x) = parseReal(token, default_x);
		(*y) = parseReal(token, default_y);
		(*z) = parseReal(token, default_z);
	}

	void ModelLoader_obj::parseV(float *x, float *y, float *z, float *w,
		const char **token, const double default_x,
		const double default_y,
		const double default_z,
		const double default_w) 
	{
		(*x) = parseReal(token, default_x);
		(*y) = parseReal(token, default_y);
		(*z) = parseReal(token, default_z);
		(*w) = parseReal(token, default_w);
	}

	bool ModelLoader_obj::parseOnOff(const char **token, bool default_value) {
		(*token) += strspn((*token), " \t");
		const char *end = (*token) + strcspn((*token), " \t\r");

		bool ret = default_value;
		if ((0 == strncmp((*token), "on", 2))) 
		{
			ret = true;
		}
		else if ((0 == strncmp((*token), "off", 3))) 
		{
			ret = false;
		}

		(*token) = end;
		return ret;
	}

	bool ModelLoader_obj::fixIndex(int idx, int n, int *ret) 
	{
		if (!ret) {
			return false;
		}

		if (idx > 0) {
			(*ret) = idx - 1;
			return true;
		}

		if (idx == 0) {
			// zero is not allowed according to the spec.
			return false;
		}

		if (idx < 0) {
			(*ret) = n + idx;  // negative value = relative
			return true;
		}

		return false;  // never reach here.
	}

	std::string ModelLoader_obj::parseString(const char **token) 
	{
		std::string s;
		(*token) += strspn((*token), " \t");
		size_t e = strcspn((*token), " \t\r");
		s = std::string((*token), &(*token)[e]);
		(*token) += e;
		return s;
	}

	int ModelLoader_obj::parseInt(const char **token) 
	{
		(*token) += strspn((*token), " \t");
		int i = atoi((*token));
		(*token) += strcspn((*token), " \t\r");
		return i;
	}

	bool ModelLoader_obj::parseTriple(const char **token, int vsize, int vnsize, int vtsize, vertex_index *ret) 
	{
		if (!ret) 
		{
			return false;
		}

		vertex_index vi(-1);

		if (!fixIndex(atoi((*token)), vsize, &(vi.v_idx))) 
		{
			return false;
		}

		(*token) += strcspn((*token), "/ \t\r");
		if ((*token)[0] != '/') 
		{
			(*ret) = vi;
			return true;
		}
		(*token)++;

		// i//k
		if ((*token)[0] == '/') 
		{
			(*token)++;

			if (!fixIndex(atoi((*token)), vnsize, &(vi.vn_idx))) 
			{
				return false;
			}

			(*token) += strcspn((*token), "/ \t\r");
			(*ret) = vi;
			return true;
		}

		// i/j/k or i/j
		if (!fixIndex(atoi((*token)), vtsize, &(vi.vt_idx))) 
		{
			return false;
		}

		(*token) += strcspn((*token), "/ \t\r");
		if ((*token)[0] != '/') 
		{
			(*ret) = vi;
			return true;
		}

		// i/j/k
		(*token)++;  // skip '/'
		if (!fixIndex(atoi((*token)), vnsize, &(vi.vn_idx))) 
		{
			return false;
		}
		(*token) += strcspn((*token), "/ \t\r");

		(*ret) = vi;

		return true;
	}

	bool ModelLoader_obj::parseTextureNameAndOption(std::string *texname, const char *linebuf, const bool is_bump) {

		// @todo { write more robust lexer and parser. }
		bool found_texname = false;
		std::string texture_name;

		// Fill with default value for texopt.
		if (is_bump) {
			//texopt->imfchan = 'l';
		}
		else {
			//texopt->imfchan = 'm';
		}
		//texopt->bump_multiplier = 1.0f;
		//texopt->clamp = false;
		//texopt->blendu = true;
		//texopt->blendv = true;
		//texopt->sharpness = 1.0f;
		//texopt->brightness = 0.0f;
		//texopt->contrast = 1.0f;
		//texopt->origin_offset[0] = 0.0f;
		//texopt->origin_offset[1] = 0.0f;
		//texopt->origin_offset[2] = 0.0f;
		//texopt->scale[0] = 1.0f;
		//texopt->scale[1] = 1.0f;
		//texopt->scale[2] = 1.0f;
		//texopt->turbulence[0] = 0.0f;
		//texopt->turbulence[1] = 0.0f;
		//texopt->turbulence[2] = 0.0f;
		//texopt->type = TEXTURE_TYPE_NONE;

		const char *token = linebuf;  // Assume line ends with NULL

		while (!IS_NEW_LINE((*token))) {
			token += strspn(token, " \t");  // skip space
			if ((0 == strncmp(token, "-blendu", 7)) && IS_SPACE((token[7]))) {
				token += 8;
				//texopt->blendu = parseOnOff(&token, /* default */ true);
			}
			else if ((0 == strncmp(token, "-blendv", 7)) && IS_SPACE((token[7]))) {
				token += 8;
				//texopt->blendv = parseOnOff(&token, /* default */ true);
			}
			else if ((0 == strncmp(token, "-clamp", 6)) && IS_SPACE((token[6]))) {
				token += 7;
				//texopt->clamp = parseOnOff(&token, /* default */ true);
			}
			else if ((0 == strncmp(token, "-boost", 6)) && IS_SPACE((token[6]))) {
				token += 7;
				//texopt->sharpness = parseReal(&token, 1.0);
			}
			else if ((0 == strncmp(token, "-bm", 3)) && IS_SPACE((token[3]))) {
				token += 4;
				//texopt->bump_multiplier = parseReal(&token, 1.0);
			}
			else if ((0 == strncmp(token, "-o", 2)) && IS_SPACE((token[2]))) {
				token += 3;
				//parseReal3(&(texopt->origin_offset[0]), &(texopt->origin_offset[1]), &(texopt->origin_offset[2]), &token);
			}
			else if ((0 == strncmp(token, "-s", 2)) && IS_SPACE((token[2]))) {
				token += 3;
				//parseReal3(&(texopt->scale[0]), &(texopt->scale[1]), &(texopt->scale[2]), &token, 1.0, 1.0, 1.0);
			}
			else if ((0 == strncmp(token, "-t", 2)) && IS_SPACE((token[2]))) {
				token += 3;
				//parseReal3(&(texopt->turbulence[0]), &(texopt->turbulence[1]), &(texopt->turbulence[2]), &token);
			}
			else if ((0 == strncmp(token, "-type", 5)) && IS_SPACE((token[5]))) {
				token += 5;
				//texopt->type = parseTextureType((&token), TEXTURE_TYPE_NONE);
			}
			else if ((0 == strncmp(token, "-imfchan", 8)) && IS_SPACE((token[8]))) {
				token += 9;
				token += strspn(token, " \t");
				const char *end = token + strcspn(token, " \t\r");
				if ((end - token) == 1) {  // Assume one char for -imfchan
					//texopt->imfchan = (*token);
				}
				token = end;
			}
			else if ((0 == strncmp(token, "-mm", 3)) && IS_SPACE((token[3]))) {
				token += 4;
				//parseReal2(&(texopt->brightness), &(texopt->contrast), &token, 0.0, 1.0);
			}
			else {
				// Assume texture filename
#if 0
				size_t len = strcspn(token, " \t\r");  // untile next space
				texture_name = std::string(token, token + len);
				token += len;

				token += strspn(token, " \t");  // skip space
#else
				// Read filename until line end to parse filename containing whitespace
				// TODO(syoyo): Support parsing texture option flag after the filename.
				texture_name = std::string(token);
				token += texture_name.length();
#endif

				found_texname = true;
			}
		}

		if (found_texname) {
			(*texname) = texture_name;
			return true;
		}
		else {
			return false;
		}
	}
}
