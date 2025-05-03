#pragma once

#include <string_view>
#include <iostream>

namespace crayon
{
	namespace glsl
	{
		enum class TokenType
		{
			UNDEFINED = -1,

			// Type Qualifiers

			LAYOUT,
			IN, OUT,

			// Types

			VOID, INT, VEC3, VEC4,

			// Operators

			EQUAL,
			PLUS, DASH, STAR, SLASH,

			// Constants and other "primaries"

			IDENTIFIER,

			INTCONSTANT, FLOATCONSTANT,

			// Punctuation marks

			LEFT_PAREN, RIGHT_PAREN,
			LEFT_BRACE, RIGHT_BRACE,
			DOT, COMMA, SEMICOLON,

			TOKEN_NUM
		};

		struct Token
		{
			std::string_view lexeme{ };
			TokenType        tokenType{ TokenType::UNDEFINED };
			int              line{ 0 };
			int              column{ 0 };
		};

		void PrintToken(std::ostream& out, const Token& token);
	}
}