#pragma once

#include "GLSL/Token.h"

#include <vector>
#include <unordered_map>

namespace crayon
{
	namespace glsl
	{
		struct LexerConfig
		{
			// const ErrorReporter* errorReporter{ nullptr };
			const std::unordered_map<std::string_view, TokenType>* keywords{ nullptr };
		};

		struct LexerState
		{
			uint32_t start{ 0 };
			uint32_t current{ 0 };

			uint32_t line{ 0 };
			uint32_t column{ 0 };
		};

		class Lexer
		{
		public:

			void ScanSrcCode(const char* srcCodeData, size_t srcCodeSize, const LexerConfig& config);

			const std::vector<Token>& GetTokens() const;

		private:

			LexerState GetLexerState() const;

			void ScanToken();

			Token CreateToken() const;
			Token CreateToken(TokenType tokenType) const;

			void AddToken(TokenType tokenType);
			void AddToken(const Token& token);

			char Advance();
			char Peek() const;
			char PeekNext() const;
			bool Match(char c);

			void Number();
			void Identifier();

			void AddIntConstant();
			void AddFloatConstant();
			void AddIdOrKeyword();

			bool Alpha(char c) const;
			bool Numeric(char c) const;
			bool AlphaNumeric(char c) const;

			bool AtEnd() const;
			bool AtEndNext() const;

			std::vector<Token> tokens;

			LexerConfig config{};

			const char* srcCodeData{ nullptr };
			size_t srcCodeSize{ 0 };

			uint32_t start{ 0 };
			uint32_t current{ 0 };

			uint32_t line{ 0 };
			uint32_t column{ 0 };
		};
	}
}