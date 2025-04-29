#pragma once

#include "GLSL/Token.h"

#include <vector>

namespace crayon
{
	namespace glsl
	{
		class Lexer
		{
		public:

			void ScanSrcCode(const char* srcCodeData, size_t srcCodeSize);

			const std::vector<Token>& GetTokens();

		private:

			void ScanToken();

			Token CreateToken(TokenType tokenType);
			void AddToken(TokenType tokenType);

			char Advance();
			char Peek();
			char PeekNext();
			bool Match(char c);

			void Number();
			void AddIntegerConstant();

			bool Alpha(char c) const;
			bool Numeric(char c) const;
			bool AlphaNumeric(char c) const;

			bool AtEnd() const;
			bool AtEndNext() const;

			std::vector<Token> tokens;

			const char* srcCodeData{ nullptr };
			size_t srcCodeSize{ 0 };

			uint32_t start{ 0 };
			uint32_t current{ 0 };

			uint32_t line{ 0 };
			uint32_t column{ 0 };
		};
	}
}