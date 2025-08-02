#pragma once

#include "GLSL/Token.h"
#include "GLSL/Value.h"

#include "CmdLine/CmdLineCommon.h"

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		struct LexerConfig {
			// const ErrorReporter* errorReporter{nullptr};
			const std::unordered_map<std::string_view, TokenType>* keywords{nullptr};
			GpuApiType gpuApiType{GpuApiType::NONE};
		};

		struct LexerState {
			// Column state
			uint32_t startCol{0};
			uint32_t currentCol{0};
			// Line state
			uint32_t line{0};
			// Position in the input
			uint32_t start{0};
			uint32_t current{0};
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
			char Previous();
			bool Match(char c);
			char Consume(char c, std::string_view errMsg);

			void HandleNewLine();

			void Number();
			void DecimalNumber();
			void OctalNumber();
			void HexadecimalNumber();
			void FinishIntegerNumber(IntConstType intConstType);
			void FloatingPointNumber();

			TokenType HandleIntSuffix();
			TokenType HandleFloatSuffix();

			void String(char delim);
			void Identifier();

			void AddIntConstant(IntConstType intConstType);
			void AddUintConstant(IntConstType intConstType);
			void AddFloatConstant();
			void AddDoubleConstant();
			
			void AddIdOrKeyword();

			bool Letter(char c) const;
			bool Alpha(char c) const;
			bool DecimalDigit(char c) const;
			bool HexadecimalDigit(char c) const;
			bool OctalDigit(char c) const;
			bool AlphaNumeric(char c) const;

			bool AtEnd() const;
			bool AtEndNext() const;

			std::vector<Token> tokens;

			LexerConfig config;
			LexerState state;

			const char* srcData{nullptr};
			size_t srcSize{0};
		};
	}
}