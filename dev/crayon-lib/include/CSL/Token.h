#pragma once

#include <string_view>
#include <iostream>

namespace crayon
{
	namespace csl
	{
		enum class TokenType
		{
			UNDEFINED,

			EXCL,                      // !
			EXCL_EQUAL, EQUAL_EQUAL,   // !=, ==
			LESS, LESS_EQUAL,    //  <, <=
			GREATER, GREATER_EQUAL, //  >, >=

			EQUAL,       // =
			PLUS, MINUS, // +, -
			STAR, SLASH, // *, /

			IDENTIFIER, // abc123
			NUMBER,     // 1, -2, 3.14, 0x2A, etc.
			STRING,     // "abc123"

			// Type keywords

			VOID,
			BOOL, TRUE, FALSE,
			UNSIGNED, INT,
			FLOAT, DOUBLE,

			// Control keywords

			AMP, AMPAMP,    // &, &&
			PIPE, PIPEPIPE, // |, ||

			IF, ELSE,  // if,  else
			FOR, WHILE, // for, while

			EOF_TOK,

			TOKEN_NUM
		};

		struct Token
		{
			std::string_view lexeme{ };
			TokenType        tokenType{ TokenType::UNDEFINED };
			int              line{ 0 };
			int              column{ 0 };
			int              id{ -1 };
		};

		void PrintToken(std::ostream& out, const Token& token);
	}
}