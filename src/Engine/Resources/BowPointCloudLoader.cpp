#include "BowResources.h"
#include "BowPointCloudLoader.h"

#include <sstream>
#include <iostream>

namespace bow {
	
	#define IS_SPACE(x) (((x) == ' ') || ((x) == '\t'))
	#define IS_DIGIT(x) (static_cast<unsigned int>((x) - '0') < static_cast<unsigned int>(10))
	#define IS_NEW_LINE(x) (((x) == '\r') || ((x) == '\n') || ((x) == '\0'))

	PointCloudLoader::PointCloudLoader()
	{

	}

	PointCloudLoader::~PointCloudLoader()
	{        

	}

	void PointCloudLoader::ImportPointCloud(const char* inputData, PointCloud* outputMesh)
	{
		std::string line;
		std::istringstream dataStream(inputData);
		while (safeGetline(dataStream, line)) 
		{
			// Trim newline '\r\n' or '\n'
			if (line.size() > 0) {
				if (line[line.size() - 1] == '\n')
					line.erase(line.size() - 1);
			}

			// Skip if empty line.
			if (line.empty()) {
				continue;
			}

			// Skip leading space.
			const char *token = line.c_str();
			token += strspn(token, " \t");

			float x, y, z;
			parseReal3(&x, &y, &z, &token);
			outputMesh->m_vertices.push_back(Vector3<float>(x, y, z));
			continue;
		}

	}

	// ==========================================================
	// Private
	// ==========================================================

	// See
	// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
	std::istream& PointCloudLoader::safeGetline(std::istream &is, std::string &t)
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

	void PointCloudLoader::parseReal3(float *x, float *y, float *z,
		const char **token, const double default_x,
		const double default_y,
		const double default_z)
	{
		(*x) = parseReal(token, default_x);
		(*y) = parseReal(token, default_y);
		(*z) = parseReal(token, default_z);
	}

	float PointCloudLoader::parseReal(const char **token, double default_value)
	{
		(*token) += strspn((*token), " \t");
		const char *end = (*token) + strcspn((*token), " \t\r");
		double val = default_value;
		tryParseDouble((*token), end, &val);
		float f = static_cast<float>(val);
		(*token) = end;
		return f;
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
	bool PointCloudLoader::tryParseDouble(const char *s, const char *s_end, double *result)
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
}
