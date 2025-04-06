#pragma once

#include <string_view>
#include <iostream>

namespace csl
{
	enum class TokenType
	{
		UNDEFINED,

		EXCL,
		EXCL_EQUAL, EQUAL_EQUAL,
		LESS,       LESS_EQUAL,
		GREATER,    GREATER_EQUAL,

		EQUAL,
		PLUS, MINUS,
		STAR, SLASH,

		IDENTIFIER, // abc123
		NUMBER,     // 1, -2, 3.14, 0x2A, etc.
		STRING,     // "abc123"

		AND, OR,
		IF,  ELSE,
		FOR, WHILE,

		EOF_TOK,
	};

	struct Token
	{
		std::string_view lexeme{};
		TokenType tokenType{ TokenType::UNDEFINED };
		int line{ 0 };
		int column{ 0 };
	};

	void PrintToken(std::ostream& out, const Token& token);
}