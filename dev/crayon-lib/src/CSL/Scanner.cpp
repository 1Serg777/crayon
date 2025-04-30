#include "CSL/Scanner.h"

#include <array>
#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace crayon
{
	namespace csl
	{
		static constexpr int ERR_MSG_BUF_SIZE = 256;
		static std::array<char, ERR_MSG_BUF_SIZE> errMsgBuf;

		static constexpr std::string_view EOF_STR{ "\0" };

		void ConsoleScannerErrorReporter::ReportUnrecognizedToken(const Token& token)
		{
			std::snprintf(
				errMsgBuf.data(), ERR_MSG_BUF_SIZE,
				"Unrecognized token { lexeme: \"%s\" } on line: %d, column: %d",
				token.lexeme.data(), token.line, token.column);
		}
		void ConsoleScannerErrorReporter::ReportUnterminatedStringLiteral(const Token& token)
		{
			std::snprintf(
				errMsgBuf.data(), ERR_MSG_BUF_SIZE,
				"Unterminated string literal { lexeme: \"%s\" } on line: %d, column: %d",
				token.lexeme.data(), token.line, token.column);
		}
		void ConsoleScannerErrorReporter::ReportUnterminatedMultiLIneComment(const Token& token)
		{
			std::snprintf(
				errMsgBuf.data(), ERR_MSG_BUF_SIZE,
				"Unterminated multi-line comment starting on line: %d, column: %d",
				token.line, token.column);
		}

		void Scanner::ScanSrcCode(
			const ScannerParams& scannerParams,
			const char* srcCodeData,
			size_t srcCodeSize)
		{
			SetScannerParams(scannerParams);
			this->srcCodeData = srcCodeData;
			this->srcCodeSize = srcCodeSize;

			line = start = current = 0;

			while (!AtEnd())
			{
				start = current;
				ScanToken();
			}

			Token eofTok{};
			eofTok.lexeme = EOF_STR;
			eofTok.tokenType = TokenType::EOF_TOK;
			eofTok.column = column;
			eofTok.line = line;
			tokens.push_back(eofTok);

			this->srcCodeSize = 0;
			this->srcCodeData = nullptr;
			ClearScannerParams();
		}

		const std::vector<Token>& Scanner::GetTokens()
		{
			return tokens;
		}

		void Scanner::SetScannerParams(const ScannerParams& scannerParams)
		{
			this->errorReporter = scannerParams.errorReporter;
		}
		void Scanner::ClearScannerParams()
		{
			this->errorReporter = nullptr;
		}

		void Scanner::ScanToken()
		{
			char c = Advance();
			switch (c)
			{
			case '!':
			{
				if (Match('='))
					AddToken(TokenType::EXCL);
				else
					AddToken(TokenType::EXCL_EQUAL);
			}
			break;
			case '=':
			{
				if (Match('='))
					AddToken(TokenType::EQUAL_EQUAL);
				else
					AddToken(TokenType::EQUAL);
			}
			break;
			case '>':
			{
				if (Match('='))
					AddToken(TokenType::GREATER_EQUAL);
				else
					AddToken(TokenType::GREATER);
			}
			break;
			case '<':
			{
				if (Match('='))
					AddToken(TokenType::LESS_EQUAL);
				else
					AddToken(TokenType::LESS);
			}
			break;

			case '+':
			{
				AddToken(TokenType::PLUS);
			}
			break;
			case '-':
			{
				AddToken(TokenType::MINUS);
			}
			break;
			case '*':
			{
				AddToken(TokenType::STAR);
			}
			break;
			case '/':
			{
				uint32_t commentLineStart = line;
				uint32_t commentColumnStart = column - 1;
				if (Match('/'))
				{
					while (!AtEnd() && Peek() != '\n')
						Advance();
				}
				else if (Match('*'))
				{
					while (!AtEnd())
					{
						char c = Peek();
						if (c == '\n')
							line++;
						if (c == '*' && !AtEndNext() && PeekNext() == '/')
						{
							// Advance(); Advance();
							current += 2;
							break;
						}
						Advance();
					}

					if (AtEnd())
					{
						Token token{};
						token.line = commentLineStart;
						token.column = commentColumnStart;
						errorReporter->ReportUnterminatedMultiLIneComment(token);
					}
				}
				else
				{
					AddToken(TokenType::SLASH);
				}
			}
			break;

			case '\n':
			{
				column = 0;
				line++;
			}
			break;

			case ' ':
			case '\r':
			case '\t':
			{
				// do nothing
			}
			break;

			case '\0':
			{
				AddToken(TokenType::EOF_TOK);
			}
			break;

			case '\"':
			{
				String('\"');
			}
			break;
			case '\'':
			{
				String('\'');
			}
			break;

			default:
			{
				if (Alpha(c))
				{
					Identifier();
				}
				else if (Numeric(c))
				{
					Number();
				}
				else
				{
					errorReporter->ReportUnrecognizedToken(ProduceToken(TokenType::UNDEFINED));
				}
			}
			break;
			}
		}

		Token Scanner::ProduceToken(TokenType tokenType)
		{
			Token token{};
			token.tokenType = tokenType;
			token.lexeme = std::string_view{ srcCodeData + start, current - start };
			return token;
		}

		void Scanner::AddToken(TokenType tokenType)
		{
			tokens.push_back(ProduceToken(tokenType));
		}
		void Scanner::AddNumberToken()
		{
			AddToken(TokenType::NUMBER);
			Token& numTok = tokens[tokens.size() - 1];
			// TODO
		}
		void Scanner::AddIdentifierToken()
		{
			AddToken(TokenType::IDENTIFIER);
			Token& numTok = tokens[tokens.size() - 1];
			// TODO
		}
		void Scanner::AddStringToken()
		{
			AddToken(TokenType::STRING);
			Token& numTok = tokens[tokens.size() - 1];
			// literalStartPos = 1
			// [literalStartPos, numTok.lexeme.sixe() - literalStartPos - 1)
			numTok.lexeme = numTok.lexeme.substr(1, numTok.lexeme.size() - 2); // Remove the quotes
		}

		char Scanner::Advance()
		{
			column++;
			return srcCodeData[current++];
		}
		char Scanner::Peek()
		{
			if (AtEnd())
				return '\0';
			else
				return srcCodeData[current];
		}
		char Scanner::PeekNext()
		{
			if (AtEndNext())
				return '\0';
			else
				return srcCodeData[current + 1];
		}
		bool Scanner::Match(char c)
		{
			if (Peek() == c)
			{
				Advance();
				return true;
			}

			return false;
		}

		void Scanner::Number()
		{
			while (Numeric(Peek()))
			{
				Advance();
			}

			// Check if it's a floating-point number
			if (Peek() == '.' && Numeric(PeekNext()))
			{
				Advance(); // Advance past the dot
				while (Numeric(Peek()))
				{
					Advance();
				}

				// Add the floating-point number token
				// TODO
			}

			// Add the integer number token
			// TODO

			AddNumberToken();
		}
		void Scanner::String(char delim)
		{
			char c{ '\0' };
			while (!AtEnd() && (c = Peek()) != '\n' && c != delim)
			{
				Advance();
			}

			if (AtEnd() || c == '\n')
			{
				current++; // Avoid including the openning quote
				errorReporter->ReportUnterminatedStringLiteral(ProduceToken(TokenType::STRING));
			}

			// 'consume' or 'expect' the delimiter character
			if (!Match(delim))
				throw std::logic_error{ "The string delimeter character must match!" };

			AddStringToken();
		}
		void Scanner::Identifier()
		{
			while (AlphaNumeric(Peek()))
			{
				Advance();
			}

			AddIdentifierToken();
		}

		bool Scanner::Alpha(char c) const
		{
			return (c >= 'a' && c <= 'z' ||
				c >= 'A' && c <= 'Z' ||
				c == '_');
		}
		bool Scanner::Numeric(char c) const
		{
			return (c >= '0' && c <= '9');
		}
		bool Scanner::AlphaNumeric(char c) const
		{
			return Alpha(c) || Numeric(c);
		}

		bool Scanner::AtEnd() const
		{
			return current >= srcCodeSize;
		}
		bool Scanner::AtEndNext() const
		{
			return current + 1 >= srcCodeSize;
		}
	}
}