#pragma once

#include <string_view>
#include <iostream>

namespace crayon {
	namespace glsl {

		enum class TokenType {
			UNDEFINED = -1,

			// Type Qualifiers:
			// Layout Qualifiers
			LAYOUT,
			// Storage Qualifiers
			IN, OUT,
			// Precision qualifiers
			HIGH_PRECISION, MEDIUM_PRECESION, LOW_PRECISION,
			// Interpolation qualifiers
			SMOOTH, FLAT, NOPERSPECTIVE,
			// Invariant qualifiers
			INVARIANT,
			// Precise qualifier
			PRECISE,

			// Types:
			STRUCT,
			VOID,
			// Scalar basic types:
			BOOL, FLOAT, INT, UINT, DOUBLE,
			// Vector types:
			BVEC2, /*Add more vector types*/
			VEC2, VEC3, VEC4,
			DVEC2, DVEC3, DVEC4,
			// Matrix types:
			MAT2, /*Add more matrix types*/
			DMAT4X4,
			// ... add more types later ...
			// Opaque types:
			SAMPLER2D,
			UIMAGE2DMSARRAY,

			// Operators:
			EQUAL,
			PLUS, DASH, STAR, SLASH,

			// Constants and other "primaries":
			IDENTIFIER,
			INTCONSTANT, UINTCONSTANT,
			FLOATCONSTANT, DOUBLECONSTANT,

			// Punctuation marks:
			LEFT_PAREN, RIGHT_PAREN,
			LEFT_BRACKET, RIGHT_BRACKET,
			LEFT_BRACE, RIGHT_BRACE,
			DOT, COMMA, SEMICOLON,

			TOKEN_NUM
		};

		struct Token {
			std::string_view lexeme   {};
			TokenType        tokenType{TokenType::UNDEFINED};
			int              line     {0};
			int              column   {0};
		};

		void PrintToken(std::ostream& out, const Token& token);
		std::string_view TokenTypeToStr(TokenType tokenType);
	
	}
}