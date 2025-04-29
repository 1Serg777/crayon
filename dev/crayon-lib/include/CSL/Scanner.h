#pragma once

#include "CSL/Token.h"

#include <cstdint>
#include <stdexcept>
#include <memory>
#include <vector>

namespace crayon
{
	namespace csl
	{
		class ScannerErrorReporter
		{
		public:

			virtual void ReportUnrecognizedToken(const Token& token) {};
			virtual void ReportUnterminatedStringLiteral(const Token& token) {};
			virtual void ReportUnterminatedMultiLIneComment(const Token& token) {};
		};

		class ConsoleScannerErrorReporter : public ScannerErrorReporter
		{
		public:

			void ReportUnrecognizedToken(const Token& token) override;
			void ReportUnterminatedStringLiteral(const Token& token) override;
			void ReportUnterminatedMultiLIneComment(const Token& token) override;
		};

		struct ScannerParams
		{
			ScannerErrorReporter* errorReporter{ nullptr };
		};

		class Scanner
		{
		public:

			void ScanSrcCode(
				const ScannerParams& scannerParams,
				const char* srcCodeData,
				size_t srcCodeSize);

			const std::vector<Token>& GetTokens();

		private:

			void SetScannerParams(const ScannerParams& scannerParams);
			void ClearScannerParams();

			void ScanToken();

			Token ProduceToken(TokenType tokenType);

			void AddToken(TokenType tokenType);
			void AddNumberToken();
			void AddIdentifierToken();
			void AddStringToken();

			char Advance();
			char Peek();
			char PeekNext();
			bool Match(char c);

			void Number();
			void String(char delim);
			void Identifier();

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

			ScannerErrorReporter* errorReporter{ nullptr };
		};
	}
}