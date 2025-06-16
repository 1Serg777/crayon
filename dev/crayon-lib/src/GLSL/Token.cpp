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
			"BOOL", "FLOAT", "INT", "UINT", "DOUBLE",
			// Vector types:
			"BVEC2", /*Add more vector types*/
			"VEC2", "VEC3", "VEC4",
			"DVEC2", "DVEC3", "DVEC4",
			// Matrix types:
			"MAT2", /*Add more matrix types*/
			"DMAT4X4",
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