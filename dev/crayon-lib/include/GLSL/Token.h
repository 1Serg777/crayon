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

			// Unary and Binary operators

			PLUS, DASH, STAR, SLASH,

			// Primary expressions

			INTCONSTANT,
			// LEFT_PAREN, RIGHT_PAREN,

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