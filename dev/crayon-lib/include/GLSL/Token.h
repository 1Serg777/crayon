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
			VOID, FLOAT, INT, VEC3, VEC4,
			// ... add more types later ... 
			UIMAGE2DMSARRAY,

			// Operators:
			EQUAL,
			PLUS, DASH, STAR, SLASH,

			// Constants and other "primaries":
			IDENTIFIER,
			INTCONSTANT, FLOATCONSTANT,

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
	
	}
}