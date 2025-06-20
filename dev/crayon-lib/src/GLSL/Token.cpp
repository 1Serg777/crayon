#include "GLSL/Token.h"

#include <array>

namespace crayon {
	namespace glsl {

		constexpr std::array<std::string_view,
							 static_cast<size_t>(TokenType::TOKEN_NUM)> tokenNames{
			// Type Qualifiers:
			// Layout Qualifiers
			"LAYOUT",
			// Storage Qualifiers
			"IN", "OUT", "UNIFORM", "BUFFER",
			// Precision qualifiers
			"HIGH_PRECISION", "MEDIUM_PRECESION", "LOW_PRECISION",
			// Interpolation qualifiers
			"SMOOTH", "FLAT", "NOPERSPECTIVE",
			// Invariant qualifiers
			"INVARIANT",
			// Precise qualifier
			"PRECISE",

			// Types:
			"STRUCT",
			"VOID",
			// Scalar basic types:
			"BOOL", "INT", "UINT", "FLOAT", "DOUBLE",
			// Vector types:
			"BVEC2", "IVEC2", "UVEC2", "VEC2", "DVEC2",
			"BVEC3", "IVEC3", "UVEC3", "VEC3", "DVEC3",
			"BVEC4", "IVEC4", "UVEC4", "VEC4", "DVEC4",
			// Matrix types:
			"MAT2",  "MAT3",  "MAT4",
			"DMAT2", "DMAT3", "DMAT4",
			"MAT2X2", "DMAT2X2", "MAT2X3", "DMAT2X3", "MAT2X4", "DMAT2X4",
			"MAT3X2", "DMAT3X2", "MAT3X3", "DMAT3X3", "MAT3X4", "DMAT3X4",
			"MAT4X2", "DMAT4X2", "MAT4X3", "DMAT4X3", "MAT4X4", "DMAT4X4",
			// ... add more types later ...
			// Opaque types:
			"SAMPLER2D",
			"UIMAGE2DMSARRAY",

			// Operators:
			"EQUAL",
			"PLUS", "DASH", "STAR", "SLASH",

			// Constants and other "primaries":
			"IDENTIFIER",
			"INTCONSTANT", "UINTCONSTANT",
			"FLOATCONSTANT", "DOUBLECONSTANT",

			// Punctuation marks:
			"LEFT_PAREN", "RIGHT_PAREN",
			"LEFT_BRACKET", "RIGHT_BRACKET",
			"LEFT_BRACE", "RIGHT_BRACE",
			"DOT", "COMMA", "SEMICOLON",
		};

		void PrintToken(std::ostream& out, const Token& token) {
			out << "{" << "'" << token.lexeme << "'" << ", " 
				<< "[" << token.line << ":" << token.column << "]" << "}";
		}
		std::string_view TokenTypeToStr(TokenType tokenType) {
			if (tokenType == TokenType::UNDEFINED)
				return "UNDEFINED";
			return tokenNames[static_cast<size_t>(tokenType)];
		}

	}
}