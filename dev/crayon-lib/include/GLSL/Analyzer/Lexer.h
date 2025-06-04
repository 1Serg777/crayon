#pragma once

#include "GLSL/Token.h"
#include "GLSL/Value.h"

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		struct LexerConfig {
			// const ErrorReporter* errorReporter{nullptr};
			const std::unordered_map<std::string_view, TokenType>* keywords{nullptr};
		};

		struct LexerState {
			uint32_t start{0};
			uint32_t current{0};
			uint32_t line{0};
			uint32_t column{0};
		};

		class Lexer {
		public:
			void Scan(const char* srcData, size_t srcSize, const LexerConfig& config);

			const Token* GetTokenData() const;
			size_t GetTokenSize() const;

		private:
			void ClearState();
			LexerState GetState() const;

			void ScanToken();

			Token CreateToken() const;
			Token CreateToken(TokenType tokenType) const;

			void AddToken(TokenType tokenType);
			void AddToken(const Token& token);

			char Advance();
			void PutBack();
			char Peek() const;
			char PeekNext() const;
			bool Match(char c);
			char Previous();

			void HandleNewLine();

			void Number();
			void DecimalNumber();
			void OctalNumber();
			void HexadecimalNumber();
			void FloatingPointNumber();

			void Identifier();

			void AddIntConstant(IntConstType intConstType);
			void AddFloatConstant();
			void AddIdOrKeyword();

			bool Alpha(char c) const;
			bool DecimalDigit(char c) const;
			bool HexadecimalDigit(char c) const;
			bool OctalDigit(char c) const;
			bool AlphaNumeric(char c) const;

			bool AtEnd() const;
			bool NextAtEnd() const;

			std::vector<Token> tokens;

			LexerConfig config;
			LexerState state;

			const char* srcData{nullptr};
			size_t srcSize{0};
		};
	}
}